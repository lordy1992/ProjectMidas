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

SequenceStatus GestureSeqRecorder::progressSequenceTime(int delta)
{

}

SequenceStatus GestureSeqRecorder::handleRest(ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    response.responseType = ResponseType::NONE;

    if (activeSequences.size() != 0)
    {
        status = ensureSameState(state);
        if (status == SequenceStatus::SUCCESS)
        {
            status = progressActiveHoldSequences(state, response);
        }
    }
    else
    {
        status = findRestSeq(state, response);

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

SequenceStatus GestureSeqRecorder::progressActiveHoldSequences(ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;

    bool poseWasHeld = (holdGestTimer > 0 ? false : true);

    std::list<sequenceInfo*>::iterator it = activeSequences.begin();
    while (it != activeSequences.end())
    {
        unsigned int seqProg = (*it)->progress;
        if ((*it)->seq.at(seqProg).holdRequired)
        {
            if (poseWasHeld)
            {
                if (seqProg == (*it)->seq.size() - 1)
                {
                    // completed hold sequence! (by releasing the last hold)
                    response = (*it)->sequenceResponse;
                    break;
                }
                else
                {
                    // increment progress so that the proper pose gets verified on the next called to progressActiveSequences
                    (*it)->progress++;
                }
            }
            else
            {
                (*it)->progress = 0;
                std::list<sequenceInfo*>::iterator itCopy = it; // also want to do this on callback func, so that sequence is cleared in real time (specially from GUI)  TODO
                activeSequences.erase(itCopy);
            }
        }
        else
        {
            if (poseWasHeld)
            {
                // clear the sequences that were supposed to hold at this step.
                (*it)->progress = 0;
                std::list<sequenceInfo*>::iterator itCopy = it;
                activeSequences.erase(itCopy);
            }
            // else nothing, this is fine.
        }
        it++;
    }

    return status;
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
    progressBaseTime = now;

    std::list<sequenceInfo*>::iterator it = activeSequences.begin();
    while (it != activeSequences.end())
    {
        unsigned int seqProg = (*it)->progress;
        if ((*it)->seq.at(seqProg).holdRequired)
        {
            // special case when holdRequired flag set. This means we need to check
            // if the pose was held for long enough AND if the pose being transitioned
            // into right now is the NEXT pose in the sequence (or if the held one was the 
            // final value and the response should be returned)
            ********************************************************************************************************************************************************************************
            ////// I'm here so far! <-- Jorden TODO jan 13, 938 pm, I NEED to set ALL types of sequences as active when first hit, that was the whole point!!! ********************************************
            // Then either the rest-gesture or timer acts as the chopping block ****************************************************************************************

            if ((seqProg == (*it)->seq.size() - 1) &&
                (gesture == (*it)->seq.at(seqProg).type))
            {
                // PROBLEM -- JORDEN , completion occurs on a REST, not a progress (as this clearly is...) !
                // ... so... do nothing here... as ... this shouldnt happen...

                // 01 13 15 txt vomit:
                // should have it so that on the rest function, the progress is incremented (if it should)
                // and that in this function, we're only CONFIRMING that the next gesture that's on the list is actually hit.
                // this is because you MUST implicitely rest inbetween poses... so ... I suppose poseWasHeld is a silly metric here... should only care about 
                // hold required. // Jorden TODO important. Need to handle this first. The code is a mess now... but, this is the requirement
            }
            if ((seqProg + 1 < (*it)->seq.size()) &&
                (gesture == (*it)->seq.at(seqProg + 1).type))
            {
                (*it)->progress++;
                holdGestTimer = REQ_HOLD_TIME; // reset count on any progression
                if ((*it)->progress == (*it)->seq.size())
                {
                    // found a complete sequence!
                    // NOTE: This is guaranteed to be the ONLY sequence that will match the input
                    // gesture, due to how sequences are registered. (it is a pre-condition for this function).

                    response = (*it)->sequenceResponse;
                    break;
                }
                it++;
            }


            // loop through all non-"holdRequired" sequences and cancel them TODO
            // ...

        }
        else
        {
            // standard case
            if ((seqProg < (*it)->seq.size()) &&
                (gesture == (*it)->seq.at(seqProg).type))
            {
                (*it)->progress++;
                holdGestTimer = REQ_HOLD_TIME; // reset count on any progression
                if ((*it)->progress == (*it)->seq.size())
                {
                    // found a complete sequence!
                    // NOTE: This is guaranteed to be the ONLY sequence that will match the input
                    // gesture, due to how sequences are registered. (it is a pre-condition for this function).

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

            // loop through all "holdRequired" sequences and cancel them TODO
            // ...
        }


        ////////////////////////////////////

        
        if ((seqProg < (*it)->seq.size()) && 
            (gesture == (*it)->seq.at(seqProg).type) &&
            (poseWasHeld == (*it)->seq.at(seqProg).holdRequired))
        {
            (*it)->progress++;
            holdGestTimer = REQ_HOLD_TIME; // reset count on any progression
            if ((*it)->progress == (*it)->seq.size())
            {
                // found a complete sequence!
                // NOTE: This is guaranteed to be the ONLY sequence that will match the input
                // gesture, due to how sequences are registered. (it is a pre-condition for this function).

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
        if (it->seq.size() >= 0)
        {
            if (it->seq.at(0).type == gesture)
            {
                // found sequence to activate!

                // 4 possible cases:
                // 1) If the sequence is only size one and is not holdable, then the response can be 
                // set, as the sequence is complete. 
                // 2) If the sequence is only size one and IS holdable, it is not progressed, but is still
                // placed on activeSequences, with holdGestTimer being reset
                // 3) > size 1, not holdable: standard case; increment progress and put in active
                // 4) > size 1, holdable: non-standard case; put in active, but don't increment
                if (it->seq.size() == 1)
                {
                    if (it->seq.at(0).holdRequired == true)
                    {
                        activeSequences.push_back(&(*it));
                        printStatus(true);
                    }
                    else
                    {
                        response = it->sequenceResponse;
                        status = SequenceStatus::SUCCESS;
                        break;
                    }
                } 
                else
                {
                    if (it->seq.at(0).holdRequired == true)
                    {
                        activeSequences.push_back(&(*it));
                        printStatus(true);
                    }
                    else
                    {
                        it->progress++;
                        activeSequences.push_back(&(*it));
                        printStatus(true);
                    }
                }

                holdGestTimer = REQ_HOLD_TIME; // set count on any progression
            }
        }
    }

    seqList = NULL;
    return status;
}

SequenceStatus GestureSeqRecorder::findRestSeq(ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    sequenceList *seqList = (*seqMapPerMode)[state.getMode()];

    // Loop through all possible sequences in this mode, and activate any that
    // have a matching first gesture.
    for (sequenceList::iterator it = seqList->begin(); it != seqList->end(); it++)
    {
        if (it->seq.size() >= 0)
        {
            if (it->seq.at(0) == Pose::Type::rest)
            {
                // found sequence to activate!
                if (it->seq.size() == 1)
                {
                    response = it->sequenceResponse;
                    status = SequenceStatus::SUCCESS;
                    break;
                }
                else
                {
                    // recall rest sequences can ONLY be size 1.
                    status = SequenceStatus::INVALID_SEQUENCE;
                    break;
                }
            }
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
                std::cout << ", Next Gesture: " << PoseTypeToString((*it)->seq.at(progress));
                emitString += ", Next Gesture: " + PoseTypeToString((*it)->seq.at(progress));
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

SequenceStatus GestureSeqRecorder::holdSequenceStatus(sequenceResponse& response)
{
    response.responseType = ResponseType::NONE;

    if (holdGestData.valid) 
    {
        if (holdGestData.remainingTime <= 0)
        {
            response = holdGestData.seqInfo.sequenceResponse;
        }
    }

    return SequenceStatus::SUCCESS;
}

void GestureSeqRecorder::decHoldGestDataTime(int delta)
{
    holdGestData.remainingTime -= delta;
}