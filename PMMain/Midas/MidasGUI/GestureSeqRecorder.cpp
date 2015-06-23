#include "GestureSeqRecorder.h"
#include "MidasMain.h"

unsigned int sequenceInfo::counter = 0;

GestureSeqRecorder::GestureSeqRecorder(ControlState* controlStateHandle, MainGUI* mainGuiHandle, SequenceImageManager imageManager)
    : prevState(midasMode::LOCK_MODE), progressMaxDeltaTime(DEFAULT_PROG_MAX_DELTA), progressBaseTime(clock()),
    holdGestTimer(REQ_HOLD_TIME), mainGui(mainGuiHandle),
    controlStateHandle(controlStateHandle), prevPose(Pose::rest), imageManager(imageManager)
{
    seqMapPerMode = new sequenceMapPerMode();

    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        (*seqMapPerMode)[mm] = new sequenceList();
    }

    connectGuiSignals();

    prevShowAll = signaller.getShowAll();
    timeBasedPrevState = controlStateHandle->getMode();
    updateGuiSequences();
}

GestureSeqRecorder::~GestureSeqRecorder()
{
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        delete (*seqMapPerMode)[mm];
    }
    delete seqMapPerMode;
}

void GestureSeqRecorder::unregisterAll()
{
    this->emptyActiveSequences();

    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        (*seqMapPerMode)[mm]->clear();
    }
}

SequenceStatus GestureSeqRecorder::registerSequence(midasMode mode, sequence seq, CommandData seqResponse, std::string name)
{
    sequenceInfo seqInfo;
    seqInfo.seq = seq;
    seqInfo.sequenceResponse = seqResponse;
    seqInfo.sequenceName = name;
    SequenceStatus status = checkLegalRegister(mode, seqInfo);
    if (status != SequenceStatus::SUCCESS)
    {
        return status;
    }

    sequenceList *seqList = (*seqMapPerMode)[mode];
    seqList->push_back(seqInfo);

    seqList = NULL;

    std::vector<int> ids;
    std::vector<PoseLength> lengths;
    sequence::iterator it;

    for (it = seq.begin(); it != seq.end(); ++it)
    {
        ids.push_back(it->type);
        lengths.push_back(it->poseLen);
    }

    std::vector<sequenceImageSet> images = imageManager.formSequenceSetFromIds(ids, lengths);

    signaller.emitRegisterSequence(seqInfo.id, QString(seqInfo.sequenceName.c_str()), images);

    return SequenceStatus::SUCCESS;
}

SequenceStatus GestureSeqRecorder::progressSequence(Pose::Type gesture, ControlState state, CommandData& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    response.type = commandType::NONE;

    prevPose = gesture;

    if (activeSequences.size() != 0)
    {
        status = ensureSameState(state);
        if (status == SequenceStatus::SUCCESS)
        {
            status = progressActiveSequences(gesture, state, response);
        }       
    }
    else
    {
        status = findActivation(gesture, state, response);
        
        if (state.getMode() != prevState)
        {
            updateGuiSequences();
        }

        prevState = state.getMode();
    }

    if (response.type != commandType::NONE || status != SequenceStatus::SUCCESS)
    { 
        // if the response is not NONE, a sequence has completed. Therefore all
        // active sequences must be cleared so that all valid sequences can potentially
        // be started.
        emptyActiveSequences();

        if (response.type != commandType::NONE)
        {
            std::cout << "GestureSeqRecorder returning a registered response." << std::endl;
        }
    }

    return status;
}


void GestureSeqRecorder::progressSequenceTime(int delta, CommandData& response)
{
    if (signaller.getShowAll() != prevShowAll)
    {
        updateGuiSequences();
        prevShowAll = signaller.getShowAll();
    }
    if (controlStateHandle->getMode() != timeBasedPrevState)
    {
        updateGuiSequences();
        timeBasedPrevState = controlStateHandle->getMode();
    }

    // Provide response if hold is reached and cut off 'taps' if hold is reached
    if (holdGestTimer > 0 && holdGestTimer - delta <= 0 && activeSequences.size() > 0)
    {
        // This call to progressSequenceTime indicates a 'hold'.
        // Update activeSequences now.
        activeSequencesMutex.lock();
        std::list<sequenceInfo*>::iterator it = activeSequences.begin();
        while (it != activeSequences.end())
        {
            unsigned int seqProg = (*it)->progress;

            if (seqProg < (*it)->seq.size())
            {
                // We just hit the "hold" state, handle accordingly
                if (PoseLength::HOLD == (*it)->seq.at(seqProg).poseLen)
                {
                    (*it)->progress++;
                    if ((*it)->progress == (*it)->seq.size())
                    {
                        // found a complete sequence!
                        response = (*it)->sequenceResponse;
                        break;
                    }
                    it++;
                }
                else
                {
                    if (prevPose != Pose::rest)
                    {
                        // Erase if they're still holding a non-rest pose, since
                        // that means they 'actually' hit a hold-state.
                        (*it)->progress = 0;
                        std::list<sequenceInfo*>::iterator itCopy = it;
                        it++;
                        activeSequences.erase(itCopy);
                    }
                    else
                    {
                        it++;
                    }
                }

                updateGuiSequences();
            }
        }
        activeSequencesMutex.unlock();

        if (response.type != commandType::NONE)
        {
            // if the response is not NONE, a sequence has completed. Therefore all
            // active sequences must be cleared so that all valid sequences can potentially
            // be started.
            emptyActiveSequences();

            if (response.type != commandType::NONE)
            {
                std::cout << "GestureSeqRecorder returning a registered response." << std::endl;
            }
        }
    }

    if (holdGestTimer - delta <= 0)
    {
        // ensure value doesn't loop and cause we're results if decremented too much.
        holdGestTimer = 0;
    }
    else
    {
        holdGestTimer -= delta;
    }
}


void GestureSeqRecorder::checkProgressBaseTime()
{
    clock_t now = clock();

    if (now - progressBaseTime > progressMaxDeltaTime && (activeSequences.size() > 0))
    {
        std::cout << "timed out of previous sequence attempt." << std::endl;
        
        emptyActiveSequences();
    }
}

void GestureSeqRecorder::emptyActiveSequences()
{
    // Reset all sequence info sequence stat info, and clear all references
    // to them.
    std::list<sequenceInfo*>::iterator it;
    activeSequencesMutex.lock();
    for (it = activeSequences.begin(); it != activeSequences.end(); it++)
    {
        (*it)->progress = 0;
    }

    activeSequences.clear();
    updateGuiSequences();
    activeSequencesMutex.unlock();

    std::cout << "Cleared Active Sequences." << std::endl;
}

void GestureSeqRecorder::setProgressMaxDeltaTime(clock_t newTime)
{
    progressMaxDeltaTime = newTime;
}

clock_t GestureSeqRecorder::getProgressMaxDeltaTime(void)
{
    return progressMaxDeltaTime;
}

SequenceStatus GestureSeqRecorder::checkLegalRegister(midasMode mode, sequenceInfo seqInfo) const
{
    // This is a special concession to allow rest to be used to revert modes from a mode entered by holding 
    // a gesture.
    if (seqInfo.seq.size() > 1 && seqInfo.seq.at(0).type == Pose::Type::rest)
    {
        // if rest is being registered, it cannot be part of a sequence greater than size one. 
        // Arbitrary rule, but will help with logistics.
        return SequenceStatus::INVALID_SEQUENCE;
    }

    sequence seqInQuestion = seqInfo.seq;
    sequenceList *seqList = (*seqMapPerMode)[mode];

    unsigned int compSeqIdx = 0;
    for (sequenceList::iterator it = seqList->begin(); it != seqList->end(); it++)
    {
        sequence baseSeq = it->seq;

        bool conflict = false;
        unsigned int gestureIdx = 0;

        // loop through smaller of the two sequences, as that will determine if there is a conflict
        // most efficiently
        if (baseSeq.size() < seqInQuestion.size())
        {
            for (sequence::iterator baseSeqIt = baseSeq.begin(); baseSeqIt != baseSeq.end(); baseSeqIt++)
            {
                SeqElement gestInQuestion = seqInQuestion.at(gestureIdx);
                SeqElement baseGest = *baseSeqIt;
                
                if (gestInQuestion.poseLen == PoseLength::IMMEDIATE || baseGest.poseLen == PoseLength::IMMEDIATE)
                {
                    if (gestureIdx > 1)
                    {
                        // can ONLY have length one IMMEDIATE types. This allows for very quick actions, such as clicking of a cursor,
                        // but is not fully supported.
                        return SequenceStatus::INVALID_SEQUENCE;
                    }
                    if (gestInQuestion.type == baseGest.type)
                    {
                        conflict = true;
                        break;
                    }
                }
                
                if (gestInQuestion != baseGest)
                {
                    bool conflict = false;
                    break;
                }

                gestureIdx++;
                if (gestureIdx == baseSeq.size())
                {
                    // made it to the end of the comparison and didn't find any differences!
                    conflict = true;
                }
            }
        }
        else
        {
            for (sequence::iterator seqInQIt = seqInQuestion.begin(); seqInQIt != seqInQuestion.end(); seqInQIt++)
            {
                SeqElement gestInQuestion = *seqInQIt;
                SeqElement baseGest = baseSeq.at(gestureIdx);

                if (gestInQuestion.poseLen == PoseLength::IMMEDIATE || baseGest.poseLen == PoseLength::IMMEDIATE)
                {
                    if (gestureIdx > 1)
                    {
                        // can ONLY have length one IMMEDIATE types. This allows for very quick actions, such as clicking of a cursor,
                        // but is not fully supported.
                        return SequenceStatus::INVALID_SEQUENCE;
                    }
                    if (gestInQuestion.type == baseGest.type)
                    {
                        conflict = true;
                        break;
                    }
                }

                if (gestInQuestion != baseGest)
                {
                    bool conflict = false;
                    break;
                }

                gestureIdx++;
                if (gestureIdx == seqInQuestion.size())
                {
                    // made it to the end of the comparison and didn't find any differences!
                    conflict = true;
                }
            }
        }

        if (conflict)
        {
            return SequenceStatus::CONFLICTING_SEQUENCE;
        }
    }

    // Finally, the sequence has passed through the gauntlet, and has proven itself worthy
    // of being a legal addition to the list of registered sequences.
    return SequenceStatus::SUCCESS;
}

SequenceStatus GestureSeqRecorder::ensureSameState(ControlState state)
{
    if (prevState != state.getMode())
    {
        // Try and recover from corruption by resetting prevState, and returning
        // error.
        prevState = state.getMode();
        return SequenceStatus::UNEXPECT_STATE_CHANGE;
    }

    return SequenceStatus::SUCCESS;
}

SequenceStatus GestureSeqRecorder::progressActiveSequences(Pose::Type gesture, ControlState state, CommandData& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;

    clock_t now = clock();

    if (now - progressBaseTime > progressMaxDeltaTime)
    {
        // Timeout
        emptyActiveSequences();
        return status;
    }
    if (gesture != Pose::rest)
    {
        // only update time if a new pose is input, as this flags intention 
        // is to ensure that users progress through a sequence in relative haste.
        progressBaseTime = now; 
    }

    activeSequencesMutex.lock();
    std::list<sequenceInfo*>::iterator it = activeSequences.begin();
    while (it != activeSequences.end())
    {
        unsigned int seqProg = (*it)->progress;
        if ((seqProg < (*it)->seq.size()) &&
            (PoseLength::IMMEDIATE == (*it)->seq.at(seqProg).poseLen))
        {
            // Handle IMMEDIATE uniquely, by not dealing with holdGestureTimer at all.
            int a = 5;
        }

        if (gesture == Pose::rest)
        {
            // if this is TAP gesture, and the timer is still in the 'tapping range'
            if (holdGestTimer > 0)
            {
                if ((seqProg < (*it)->seq.size()) &&
                    (PoseLength::TAP == (*it)->seq.at(seqProg).poseLen))
                {
                    // match! Progress forward :)
                    (*it)->progress++;
                    if ((*it)->progress == (*it)->seq.size())
                    {
                        // found a complete sequence!
                        response = (*it)->sequenceResponse;
                        break;
                    }
                    it++;
                }
                else
                {
                    (*it)->progress = 0;
                    std::list<sequenceInfo*>::iterator itCopy = it;
                    it++;
                    activeSequences.erase(itCopy);
                }

                updateGuiSequences();
            }
            else
            {
                it++;
                // do nothing important. This is handled in progressSequenceTime.
            }
        }
        else
        {
            // Use non-rest Poses to determine if the correct pose is being
            // performed to follow a sequence. If its not, remove it.
            if ((seqProg < (*it)->seq.size()) &&
                (gesture == (*it)->seq.at(seqProg).type))
            {
                holdGestTimer = REQ_HOLD_TIME; // reset count on any progression
                it++;
            }
            else
            {
                (*it)->progress = 0;
                std::list<sequenceInfo*>::iterator itCopy = it;
                it++;
                activeSequences.erase(itCopy);
                updateGuiSequences();
            }
        }
    }
    activeSequencesMutex.unlock();
    printStatus(true);

    return status;
}

SequenceStatus GestureSeqRecorder::findActivation(Pose::Type gesture, ControlState state, CommandData& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    sequenceList *seqList = (*seqMapPerMode)[state.getMode()];

    // Loop through all possible sequences in this mode, and activate any that
    // have a matching first gesture.
    for (sequenceList::iterator it = seqList->begin(); it != seqList->end(); it++)
    {
        if ((it->seq.size() >= 0) && 
            (it->seq.at(0).type == gesture))
        {
            clock_t now = clock();
            progressBaseTime = now;
            if (it->seq.at(0).poseLen == PoseLength::IMMEDIATE)
            {
                // Special case. Immediate isn't 'held'
                response = it->sequenceResponse;
                break;
            }

            // found sequence to activate!
            activeSequencesMutex.lock();
            activeSequences.push_back(&(*it));
            activeSequencesMutex.unlock();
            printStatus(true);

            holdGestTimer = REQ_HOLD_TIME; // set count on any progression
        }
    }

    seqList = NULL;
    return status;
}

void GestureSeqRecorder::updateGuiSequences()
{
    std::vector<sequenceProgressData> progressDataVec;
    if (signaller.getShowAll())
    {
        // Add ALL sequences registered to the current mode
        sequenceList* sl = (*seqMapPerMode)[controlStateHandle->getMode()];

        std::list<sequenceInfo>::iterator it;
        // loop once to find the max progress
        unsigned int maxProg = 0;
        for (it = sl->begin(); it != sl->end(); ++it)
        {
            if (it->progress > maxProg)
                maxProg = it->progress;
        }
        // loop twice to 'load' all sequences == progress
        for (it = sl->begin(); it != sl->end(); ++it)
        {
            if (it->progress >= maxProg)
            {
                sequenceProgressData progressData;

                progressData.seqId = it->id;
                progressData.progress = it->progress;
                progressDataVec.push_back(progressData);
            }
        }
    }
    else if (activeSequences.size() > 0)
    {
        // Add only the active sequences
        std::list<sequenceInfo*>::iterator it;

        for (it = activeSequences.begin(); it != activeSequences.end(); ++it)
        {
            sequenceProgressData progressData;

            progressData.seqId = (*it)->id;
            progressData.progress = (*it)->progress;
            progressDataVec.push_back(progressData);
        }
    }
    signaller.emitShowSequences(progressDataVec);
}

void GestureSeqRecorder::printStatus(bool verbose)
{
    activeSequencesMutex.lock();
    if (verbose)
    {
        std::cout << "Active Sequences: " << std::endl;
        std::list<sequenceInfo*>::iterator it = activeSequences.begin();
        while (it != activeSequences.end())
        {
            std::string activeSeqName = (*it)->sequenceResponse.name;
            unsigned int progress = (*it)->progress;
            unsigned int progressGoal = (*it)->seq.size();

            std::string emitString;
            std::cout << "name: \"" << activeSeqName << "\", Status: " << progress << "/" << progressGoal;
            emitString += "name: \"" + activeSeqName + "\", Status: " + std::to_string(progress) + "/" + std::to_string(progressGoal);
            if (progress < progressGoal)
            {
                // more gestures to perform before completion - called RIGHT after progress is incremented, so print it's
                // current value as the index... maybe change. TODO.
                std::cout << ", Next Gesture: " << PoseTypeToString((*it)->seq.at(progress).type);
                emitString += ", Next Gesture: " + PoseTypeToString((*it)->seq.at(progress).type);
            }
            std::cout << std::endl;
            emitString += "\n";

            it++;
        }
    }
    else
    {
        std::cout << "Active Sequences: ";
        std::list<sequenceInfo*>::iterator it = activeSequences.begin();
        while (it != activeSequences.end())
        {
            std::string activeSeqName = (*it)->sequenceResponse.name;

            std::cout << activeSeqName << ";";

            it++;
        }
    }
    activeSequencesMutex.unlock();
}

void GestureSeqRecorder::connectGuiSignals()
{
    if (mainGui)
    {
        mainGui->connectSignallerToSequenceDisplayer(&signaller);
    }
}