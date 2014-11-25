#include "GestureSeqRecorder.h"


GestureSeqRecorder::GestureSeqRecorder() : prevState(midasMode::LOCK_MODE), progressMaxDeltaTime(DEFAULT_PROG_MAX_DELTA), progressBaseTime(clock())
{
}

GestureSeqRecorder::GestureSeqRecorder(midasMode prevState, clock_t progressMaxDeltaTime) : prevState(prevState), progressMaxDeltaTime(progressMaxDeltaTime), progressBaseTime(clock()) 
{
}

GestureSeqRecorder::~GestureSeqRecorder()
{
}

SequenceStatus GestureSeqRecorder::registerSequence(midasMode mode, sequence seq, sequenceResponse seqResponse)
{
    SequenceStatus status = checkLegalRegister(mode, seq);
    if (status != SequenceStatus::SUCCESS)
    {
        return status;
    }

    sequenceMap seqMap = seqMapPerMode[mode];
    seqMapPerMode[mode]

}

SequenceStatus GestureSeqRecorder::progressSequence(myo::Pose::Type gesture, ControlState state, sequenceResponse& response)
{




    if (response.responseType != ResponseType::NONE) 
    { 
        // if the response is not NONE, a sequence has completed. Therefore all
        // active sequences must be cleared so that all valid sequences can potentially
        // be started.
        activeSequences.clear();
    }
}


void GestureSeqRecorder::updateProgressBaseTime()
{

}

void GestureSeqRecorder::setProgressMaxDeltaTime(clock_t newTime)
{
    progressBaseTime = newTime;
}

clock_t GestureSeqRecorder::getProgressMaxDeltaTime(void)
{
    return progressBaseTime;
}

SequenceStatus checkLegalRegister(midasMode mode, sequence seq);

SequenceStatus ensureSameState();