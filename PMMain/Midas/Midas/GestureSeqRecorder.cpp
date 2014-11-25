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
    SequenceStatus status = checkLegalRegister(mode, seq);
    if (status != SequenceStatus::SUCCESS)
    {
        return status;
    }

    sequenceList *seqList = (*seqMapPerMode)[mode];
    
    sequenceInfo seqInfo;
    seqInfo.seq = seq;
    seqInfo.sequenceResponse = seqResponse;
    seqList->push_back(seqInfo);

    seqList = NULL;
    return SequenceStatus::SUCCESS;
}

SequenceStatus GestureSeqRecorder::progressSequence(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    if (activeSequences.size() != 0)
    {
        status = progressActiveSequences(gesture, state, response);
    }
    else
    {
        status = findActivation(gesture, state, response);
    }

    if (response.responseType != ResponseType::NONE || status != SequenceStatus::SUCCESS)
    { 
        // if the response is not NONE, a sequence has completed. Therefore all
        // active sequences must be cleared so that all valid sequences can potentially
        // be started.
        activeSequences.clear();
    }

    return status;
}


void GestureSeqRecorder::updateProgressBaseTime()
{
    //TODO
}

void GestureSeqRecorder::setProgressMaxDeltaTime(clock_t newTime)
{
    progressBaseTime = newTime;
}

clock_t GestureSeqRecorder::getProgressMaxDeltaTime(void)
{
    return progressBaseTime;
}

SequenceStatus GestureSeqRecorder::checkLegalRegister(midasMode mode, sequence seq)
{
    //TODO
}

SequenceStatus GestureSeqRecorder::ensureSameState()
{
    //TODO
}

SequenceStatus GestureSeqRecorder::progressActiveSequences(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
{
    //TODO
}

SequenceStatus GestureSeqRecorder::findActivation(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
{
    SequenceStatus status = SequenceStatus::SUCCESS;
    sequenceList *seqList = (*seqMapPerMode)[state.getMode()];

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
                // NOTE: This is guarunteed to be the ONLY size one sequence that will match the input
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