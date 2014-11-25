#include "GestureSeqRecorder.h"


GestureSeqRecorder::GestureSeqRecorder() : prevState(midasMode::LOCK_MODE), progressMaxDeltaTime(DEFAULT_PROG_MAX_DELTA), progressBaseTime(clock())
{
    seqMapPerMode = new sequenceMapPerMode();

    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        (*seqMapPerMode)[mm] = new sequenceList();
    }
}

GestureSeqRecorder::GestureSeqRecorder(midasMode prevState, clock_t progressMaxDeltaTime) : prevState(prevState), progressMaxDeltaTime(progressMaxDeltaTime), progressBaseTime(clock()) 
{
    seqMapPerMode = new sequenceMapPerMode();
}

GestureSeqRecorder::~GestureSeqRecorder()
{
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
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

SequenceStatus GestureSeqRecorder::progressSequence(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    response.responseType = ResponseType::NONE;

    if (activeSequences.size() != 0)
    {
        status = ensureSameState(state);
        if (status != SequenceStatus::SUCCESS)
        {
            goto endProgress;
        }

        status = progressActiveSequences(gesture, state, response);
    }
    else
    {
        status = findActivation(gesture, state, response);
        
        prevState = state.getMode();
    }

    endProgress:
    if (response.responseType != ResponseType::NONE || status != SequenceStatus::SUCCESS)
    { 
        // if the response is not NONE, a sequence has completed. Therefore all
        // active sequences must be cleared so that all valid sequences can potentially
        // be started.
        emptyActiveSequences();
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
    sequenceList *seqList = (*seqMapPerMode)[mode];

    unsigned int idx = 0;
    for (sequenceList::iterator it = seqList->begin(); it != seqList->end(); it++)
    {
        if (it->seq.size() >= idx + 1)
        {
            // Sequence large enough to compare against.
            if (it->seq.at(idx) == seqInfo.seq.at(idx))
            {
                // Oh no! Overlap in sequence that is trying to be registered and
                // a sequence that has already been registered against this mode. 
                // Thus, it is NOT ALLOWED, as it would cause a logical inconsistency -
                //  ex: seq1 = a,b,c. seq2 = a,b,c,d,e. There is no way to tell if seq1
                //  has been executed, or if seq2 is partially done. Therefore DO NOT
                //  REGISTER.
                return SequenceStatus::CONFLICTING_SEQUENCE;
            }
        }
        // else don't care. Can't be in conflict, or else would have already returned
        idx++;
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

SequenceStatus GestureSeqRecorder::progressActiveSequences(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
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

    std::list<sequenceInfo*>::iterator it;
    for (it = activeSequences.begin(); it != activeSequences.end(); it++)
    {
        unsigned int seqProg = (*it)->progress;
        if (gesture == (*it)->seq.at(seqProg))
        {
            (*it)->progress++;
            if ((*it)->progress == (*it)->seq.size())
            {
                // found a complete sequence!
                // NOTE: This is guaranteed to be the ONLY sequence that will match the input
                // gesture, due to how sequences are registered. (it is a pre-condition for this function).

                response = (*it)->sequenceResponse;
                break;
            }
        }
        else
        {
            // TODO - verify that this is legal. MAY need to use a different structure than a list if it's not.
            (*it)->progress = 0;
            activeSequences.remove((*it));
        }
    }

    return status;
}

SequenceStatus GestureSeqRecorder::findActivation(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
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
            if (it->seq.at(0) == gesture) 
            {
                // found sequence to activate!

                // 2 cases - If the sequence is only size one, then the response can be set, as the 
                // sequence is complete. 
                // NOTE: This is guaranteed to be the ONLY size one sequence that will match the input
                // gesture, due to how sequences are registered. (it is a pre-condition for this function).
                if (it->seq.size() == 1)
                {
                    response = it->sequenceResponse;
                    status = SequenceStatus::SUCCESS;
                    break;
                } 
                else
                {
                    it->progress++;
                    // TODO ----- VERIFY that this actually persists with the correct memory 
                    // address being pointed to by activeSequences
                    activeSequences.push_back(&(*it));
                }
            }
        }
    }

    seqList = NULL;
    return status;
}