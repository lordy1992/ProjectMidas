#ifndef _GESTURE_SEQ_RECORDER_H
#define _GESTURE_SEQ_RECORDER_H

#include "MidasCommon.h"
#include "myo\myo.hpp"
#include "ControlState.h"
#include <map>
#include <vector>
#include <list>

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#endif

#ifdef USE_SIMULATOR
using namespace myoSim;
#else
using namespace myo;
#endif

#define DEFAULT_PROG_MAX_DELTA 1000 // ms

enum class SequenceStatus {
    SUCCESS,
    CONFLICTING_SEQUENCE,
    INVALID_SEQUENCE,
    UNEXPECT_STATE_CHANGE
};


enum class ResponseType {
    NONE,
    STATE_CHANGE,
    MOUSE_CMD,
    KYBRD_CMD
};

/**
* sequenceResponse defines all posible outcomes from a seequence
* successfully being completed. This will allow the user of a 
* GestureSeqRecorder to determine what action to take.
*/
struct sequenceResponse {
    ResponseType responseType;

    union responseAction
    {
        mouseCmds mouse;
        kybdCmds kybd;
        midasMode mode;
    };
};

typedef std::vector<Pose::Type> sequence;

/**
* Wrapper to tie state information to a sequence response.
*/
struct sequenceInfo {
    sequenceInfo() {
        progress = 0;
    }

    sequence seq;
    sequenceResponse sequenceResponse;
    unsigned int progress;
};

typedef std::list<sequenceInfo> sequenceList;
typedef std::map<midasMode, sequenceList*> sequenceMapPerMode;

class GestureSeqRecorder
{
public:
    GestureSeqRecorder();
    GestureSeqRecorder(midasMode prevState, clock_t progressMaxDeltaTime);
    ~GestureSeqRecorder();

    SequenceStatus registerSequence(midasMode mode, sequence seq, sequenceResponse seqResponse);

    SequenceStatus progressSequence(myo::Pose::Type gesture, ControlState state, sequenceResponse& response); // reminder -- {.... if (response.responseType != NONE) {clearAllActiveSequences();} }

    /**
    * Called to update progressBaseTime, so that a sequence can be cleared if 
    * timed out, without an asynchronous gesture update.
    */
    void updateProgressBaseTime();

    /**
    * Modifier.
    *
    * @param newTime The new progressMaxDeltaTime.
    */
    void setProgressMaxDeltaTime(clock_t newTime);

    /**
    * Accessor.
    *
    * @return Value of progressMaxDeltaTime.
    */
    clock_t getProgressMaxDeltaTime(void);

private:
    SequenceStatus checkLegalRegister(midasMode mode, sequence seq);

    // return UNEXPECT_STATE_CHANGE if state changed while active sequences open. This is unexpected
    // behaviour, so should stop all sequences and return immediately.
    SequenceStatus ensureSameState();

    // Holds all registered sequenceResponses in a layered organization.
    sequenceMapPerMode *seqMapPerMode;

    // Stores pointers to active sequences, so that progress can be tracked more efficiently.
    // Once ANY sequences are active, ONLY those sequences can potentially progress, until they 
    // are timed out.
    std::list<sequence*> activeSequences;

    midasMode prevState;

    // Base timestamp used to calculate transitions
    clock_t progressBaseTime;

    // Transition timeout value. For example, if to change a state,
    // the user is required to perform "wave in, wave out, pinky-to-thumb", 
    // then when a wave in is recorded, the user has up to "TransMaxDeltaTime"
    // milliseconds to perform a wave out, or else the whole process needs to be repeated.
    clock_t progressMaxDeltaTime;
};

#endif /* _GESTURE_SEQ_RECORDER_H */