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
    * Called to check against progressBaseTime if any sequences are active, so that a 
    // sequence can be cleared if timed out, without an asynchronous gesture update.
    */
    void checkProgressBaseTime();

    void emptyActiveSequences();

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
    SequenceStatus checkLegalRegister(midasMode mode, sequenceInfo seqInfo) const;

    // Track state information when attempting to start a sequence. Then ensure 
    // that state isn't altered until the end of a sequence, or else something
    // has become corrupt, so cleanup and return an error.
    SequenceStatus ensureSameState(ControlState state);

    SequenceStatus progressActiveSequences(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

    SequenceStatus findActivation(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

    // Holds all registered sequenceResponses in a layered organization.
    sequenceMapPerMode *seqMapPerMode;

    // Stores pointers to active sequenceInfos, so that progress can be tracked more efficiently.
    // Once ANY sequences are active, ONLY those sequences can potentially progress, until they 
    // are timed out.
    std::list<sequenceInfo*> activeSequences;

    // State info from when a sequence that has size >1 is started.
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