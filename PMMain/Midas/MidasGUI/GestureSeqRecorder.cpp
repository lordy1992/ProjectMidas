#include "GestureSeqRecorder.h"
#include "MidasMain.h"
/*
Checklist until this class is revamped properly:

check 1) SequenceStatus registerSequence(midasMode mode, sequence seq, sequenceResponse seqResponse);

check - actually read note 2) SequenceStatus progressSequence(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

note: recheck this, ensuring that case where it's supposed to be held, and it is still held doesnt cancel the sequence, and then if it's not held and it's supposed to be held, it wont progress. this seems obvious but yeah...  
3) SequenceStatus progressSequenceTime(int delta);

4) SequenceStatus handleRest(ControlState state, sequenceResponse& response);

good enough i think 5) void checkProgressBaseTime();

6) void emptyActiveSequences();

7) void setProgressMaxDeltaTime(clock_t newTime);

8) clock_t getProgressMaxDeltaTime(void);

check 9) SequenceStatus checkLegalRegister(midasMode mode, sequenceInfo seqInfo) const;

10) SequenceStatus ensureSameState(ControlState state);

check 11) SequenceStatus progressActiveSequences(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

12) SequenceStatus findActivation(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

13) SequenceStatus findRestSeq(ControlState state, sequenceResponse& response);

*/

GestureSeqRecorder::GestureSeqRecorder() : prevState(midasMode::LOCK_MODE), progressMaxDeltaTime(DEFAULT_PROG_MAX_DELTA), progressBaseTime(clock()), holdGestTimer(REQ_HOLD_TIME)
{
    seqMapPerMode = new sequenceMapPerMode();

    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        (*seqMapPerMode)[mm] = new sequenceList();
    }
}

GestureSeqRecorder::GestureSeqRecorder(midasMode prevState, clock_t progressMaxDeltaTime) : prevState(prevState), progressMaxDeltaTime(progressMaxDeltaTime), progressBaseTime(clock()), holdGestTimer(REQ_HOLD_TIME)
{
    seqMapPerMode = new sequenceMapPerMode();

    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        (*seqMapPerMode)[mm] = new sequenceList();
    }
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

SequenceStatus GestureSeqRecorder::registerSequence(midasMode mode, sequence seq, sequenceResponse seqResponse)
{
    sequenceInfo seqInfo;
    seqInfo.seq = seq;
    seqInfo.sequenceResponse = seqResponse;
    SequenceStatus status = checkLegalRegister(mode, seqInfo);
    if (status != SequenceStatus::SUCCESS)
    {
        return status;
    }

    sequenceList *seqList = (*seqMapPerMode)[mode];
    seqList->push_back(seqInfo);

    seqList = NULL;
    return SequenceStatus::SUCCESS;
}

SequenceStatus GestureSeqRecorder::progressSequence(Pose::Type gesture, ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    response.responseType = ResponseType::NONE;

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
        
        prevState = state.getMode();
    }

    if (response.responseType != ResponseType::NONE || status != SequenceStatus::SUCCESS)
    { 
        // if the response is not NONE, a sequence has completed. Therefore all
        // active sequences must be cleared so that all valid sequences can potentially
        // be started.
        emptyActiveSequences();

        if (response.responseType != ResponseType::NONE)
        {
            std::cout << "GestureSeqRecorder returning a registered response." << std::endl;
        }
    }

    return status;
}


void GestureSeqRecorder::progressSequenceTime(int delta, sequenceResponse& response)
{
    // Provide response if hold is reached and cut off 'taps' if hold is reached

    if (holdGestTimer - delta <= 0)
    {
        // This call to progressSequenceTime indicates a 'hold'.
        // Update activeSequences now.
        std::list<sequenceInfo*>::iterator it = activeSequences.begin();
        while (it != activeSequences.end())
        {
            unsigned int seqProg = (*it)->progress;

            if (seqProg < (*it)->seq.size())
            {
                // We just hit the "hold" state, handle accordingly
                if (true == (*it)->seq.at(seqProg).holdRequired)
                {
                    (*it)->progress++;
                    if ((*it)->progress == (*it)->seq.size())
                    {
                        // found a complete sequence!
                        response = (*it)->sequenceResponse;
                        break;
                    }
                }
                else
                {
                    (*it)->progress = 0;
                    std::list<sequenceInfo*>::iterator itCopy = it;
                    activeSequences.erase(itCopy);
                }
                it++;
            }
        }
    }

    holdGestTimer -= delta;
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
    for (it = activeSequences.begin(); it != activeSequences.end(); it++)
    {
        (*it)->progress = 0;
    }

    activeSequences.clear();
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
                seqElement gestInQuestion = seqInQuestion.at(gestureIdx);
                seqElement baseGest = *baseSeqIt;
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
                seqElement gestInQuestion = *seqInQIt;
                seqElement baseGest = baseSeq.at(gestureIdx);
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

SequenceStatus GestureSeqRecorder::progressActiveSequences(Pose::Type gesture, ControlState state, sequenceResponse& response)
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

    std::list<sequenceInfo*>::iterator it = activeSequences.begin();
    while (it != activeSequences.end())
    {
        unsigned int seqProg = (*it)->progress;

        if (gesture == Pose::rest)
        {
            // if this is NOT a 'hold' gesture, and the timer is still in the 'tapping range'
            if (holdGestTimer > 0)
            {
                if ((seqProg < (*it)->seq.size()) &&
                    (false == (*it)->seq.at(seqProg).holdRequired))
                {
                    // match! Progress forward :)
                    (*it)->progress++;
                    if ((*it)->progress == (*it)->seq.size())
                    {
                        // found a complete sequence!
                        response = (*it)->sequenceResponse;
                        break;
                    }
                }
                else
                {
                    (*it)->progress = 0;
                    std::list<sequenceInfo*>::iterator itCopy = it;
                    activeSequences.erase(itCopy);
                }
            }
            else
            {
                // do nothing. This is handled in progressSequenceTime.
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
            }
            else
            {
                (*it)->progress = 0;
                std::list<sequenceInfo*>::iterator itCopy = it;
                activeSequences.erase(itCopy);
            }
        }
        it++;
    }
    printStatus(true);

    return status;
}

SequenceStatus GestureSeqRecorder::findActivation(Pose::Type gesture, ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    sequenceList *seqList = (*seqMapPerMode)[state.getMode()];

    clock_t now = clock();
    progressBaseTime = now;

    // Loop through all possible sequences in this mode, and activate any that
    // have a matching first gesture.
    for (sequenceList::iterator it = seqList->begin(); it != seqList->end(); it++)
    {
        if ((it->seq.size() >= 0) && 
            (it->seq.at(0).type == gesture))
        {
            if (it->seq.at(0).type == Pose::rest)
            {
                // Special case. Rest can't be 'held'
                if (it->seq.size() == 1)
                {
                    response = it->sequenceResponse;
                }
                else
                {
                    // recall rest sequences can ONLY be size 1.
                    status = SequenceStatus::INVALID_SEQUENCE;
                }
                break;
            }

            // found sequence to activate!
            activeSequences.push_back(&(*it));
            printStatus(true);

            holdGestTimer = REQ_HOLD_TIME; // set count on any progression
        }
        
    }

    seqList = NULL;
    return status;
}


void GestureSeqRecorder::printStatus(bool verbose)
{
    if (verbose)
    {
        std::cout << "Active Sequences: " << std::endl;
        std::list<sequenceInfo*>::iterator it = activeSequences.begin();
        while (it != activeSequences.end())
        {
            std::string activeSeqName = (*it)->sequenceResponse.responseName;
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

            if (gThreadHandle)
            {
                // TODO - figure out WHY this isnt working! This value is always null here....  so was gMidasThread when tried that...
                gThreadHandle->threadEmitString(emitString);
            }

            it++;
        }
    }
    else
    {
        std::cout << "Active Sequences: ";
        std::list<sequenceInfo*>::iterator it = activeSequences.begin();
        while (it != activeSequences.end())
        {
            std::string activeSeqName = (*it)->sequenceResponse.responseName;

            std::cout << activeSeqName << ";";

            it++;
        }
    }
}


//SequenceStatus GestureSeqRecorder::findRestSeq(ControlState state, sequenceResponse& response)
//{
//    SequenceStatus status = SequenceStatus::SUCCESS;
//    sequenceList *seqList = (*seqMapPerMode)[state.getMode()];
//
//    // Loop through all possible sequences in this mode, and activate any that
//    // have a matching first gesture.
//    for (sequenceList::iterator it = seqList->begin(); it != seqList->end(); it++)
//    {
//        if (it->seq.size() >= 0)
//        {
//            if (it->seq.at(0) == Pose::Type::rest)
//            {
//                // found sequence to activate!
//                if (it->seq.size() == 1)
//                {
//                    response = it->sequenceResponse;
//                    status = SequenceStatus::SUCCESS;
//                    break;
//                }
//                else
//                {
//                    // recall rest sequences can ONLY be size 1.
//                    status = SequenceStatus::INVALID_SEQUENCE;
//                    break;
//                }
//            }
//        }
//    }
//
//    seqList = NULL;
//    return status;
//}

//SequenceStatus GestureSeqRecorder::handleRest(ControlState state, sequenceResponse& response)
//{
//    SequenceStatus status = SequenceStatus::SUCCESS;
//    response.responseType = ResponseType::NONE;
//
//    if (activeSequences.size() != 0)
//    {
//        status = ensureSameState(state);
//        if (status == SequenceStatus::SUCCESS)
//        {
//            status = progressActiveHoldSequences(state, response);
//        }
//    }
//    else
//    {
//        status = findRestSeq(state, response);
//
//        prevState = state.getMode();
//    }
//
//    if (response.responseType != ResponseType::NONE || status != SequenceStatus::SUCCESS)
//    {
//        // if the response is not NONE, a sequence has completed. Therefore all
//        // active sequences must be cleared so that all valid sequences can potentially
//        // be started.
//        emptyActiveSequences();
//
//        if (response.responseType != ResponseType::NONE)
//        {
//            std::cout << "GestureSeqRecorder returning a registered response." << std::endl;
//        }
//    }
//
//    return status;
//}
//
//SequenceStatus GestureSeqRecorder::progressActiveHoldSequences(ControlState state, sequenceResponse& response)
//{
//    SequenceStatus status = SequenceStatus::SUCCESS;
//
//    bool poseWasHeld = (holdGestTimer > 0 ? false : true);
//
//    std::list<sequenceInfo*>::iterator it = activeSequences.begin();
//    while (it != activeSequences.end())
//    {
//        unsigned int seqProg = (*it)->progress;
//        if ((*it)->seq.at(seqProg).holdRequired)
//        {
//            if (poseWasHeld)
//            {
//                if (seqProg == (*it)->seq.size() - 1)
//                {
//                    // completed hold sequence! (by releasing the last hold)
//                    response = (*it)->sequenceResponse;
//                    break;
//                }
//                else
//                {
//                    // increment progress so that the proper pose gets verified on the next called to progressActiveSequences
//                    (*it)->progress++;
//                }
//            }
//            else
//            {
//                (*it)->progress = 0;
//                std::list<sequenceInfo*>::iterator itCopy = it; // also want to do this on callback func, so that sequence is cleared in real time (specially from GUI)  TODO
//                activeSequences.erase(itCopy);
//            }
//        }
//        else
//        {
//            if (poseWasHeld)
//            {
//                // clear the sequences that were supposed to hold at this step.
//                (*it)->progress = 0;
//                std::list<sequenceInfo*>::iterator itCopy = it;
//                activeSequences.erase(itCopy);
//            }
//            // else nothing, this is fine.
//        }
//        it++;
//    }
//
//    return status;
//}