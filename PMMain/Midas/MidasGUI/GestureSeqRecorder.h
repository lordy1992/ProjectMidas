#ifndef _GESTURE_SEQ_RECORDER_H
#define _GESTURE_SEQ_RECORDER_H

#include "MidasCommon.h"
#include "myo\myo.hpp"
#include "MyoCommon.h"
#include "ControlState.h"
#include <map>
#include <vector>
#include <list>
#include <string>
#include <iostream>
// TODO!!! TEMP only -- obtaining global handle on FIRST created MidasThread (ideally the only one) and using that to emitString.
#include "MidasThread.h"
// TODO!!! TEMP only -- obtaining global handle on FIRST created MidasThread (ideally the only one) and using that to emitString.
static MidasThread *gMidasThread;

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#endif

#ifdef USE_SIMULATOR
using namespace myoSim;
#else
using namespace myo;
#endif

#define DEFAULT_PROG_MAX_DELTA 1000 // ms

#define REQ_HOLD_TIME 500 // ms

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
    sequenceResponse() {
        responseName = "";
    }

    ResponseType responseType;

    union responseAction
    {
        mouseCmds mouse;
        kybdCmds kybd;
        midasMode mode;
    };
    responseAction responseAction;

    std::string responseName;
};

struct seqElement {
    Pose::Type type;
    bool holdRequired = false;
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

    bool holdRequired = false;
};

typedef std::list<sequenceInfo> sequenceList;
typedef std::map<midasMode, sequenceList*> sequenceMapPerMode;

typedef struct HoldGestData {
    bool valid = false;
    sequenceInfo seqInfo; 
    int remainingTime = -1;
} HoldGestData;

class GestureSeqRecorder
{
public:
    /**
    * Constructors/Destructor
    */
    GestureSeqRecorder();
    GestureSeqRecorder(midasMode prevState, clock_t progressMaxDeltaTime);
    ~GestureSeqRecorder();

    /**
    * Register a sequence of gestures with this call. A registered sequence is associated with a 
    * midasMode. When in that mode, any progression of gestures from the Myo will be compared
    * to all associated and registered sequences, and if any are succesfully completed, the 
    * registered sequenceResponse will be returned to the caller.
    *
    * @param mode The midasMode that the caller is registering the sequence against.
    * @param seq The sequence (std::vector<Pose::Type>) of gestures to register
    * @param seqResponse The desired response to activate if the sequence is recognized
    * while in the registered mode.
    * @param holdRequired States that the registration is in regard to a gesture that must be held.
    * @return SequenceStatus associated status information to inform caller of success/lack there of
    */
    SequenceStatus registerSequence(midasMode mode, sequence seq, sequenceResponse seqResponse, bool holdRequired = false);

    /**
    * Given a gesture, attempt to progress through any registered sequences, that match the mode
    * of the controlState.
    * 
    * @param gesture The recorded gesture that is being used to compare with the registered sequences.
    * @param state The ControlState handle to determine the current midasMode of the system, thereby 
    * knowing which registered sequence list to search through.
    * @param response The sequenceResponse that is populated by the function. Holding a type of NONE
    * means that no sequence was completed. However, if it's not NONE, it holds the response that
    * was registered against the completed sequence.
    * @return SequenceStatus associated status information to inform caller of success/lack there of
    */ 
    SequenceStatus progressSequence(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

    SequenceStatus handleRest(ControlState state, sequenceResponse& response);

    /**
    * Called to check against progressBaseTime if any sequences are active, so that a 
    * sequence can be cleared if timed out, without an asynchronous gesture update.
    */
    void checkProgressBaseTime();

    /**
    * Empty the references to sequences that were active, and set progress of each to 0.
    */
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

    /**
    * Print out a list of all active sequences, and possibly their progress,
    * and next gesture required to proceed!
    *
    * @param verbose If true, prints progress and next gesture in sequence on top
    * of active status.
    *
    * TODO: This is the beginning of the code which overlays graphically action
    * data to the user. Just using printing temporarily.
    */
    void printStatus(bool verbose = false);

    void decHoldGestDataTime(int delta);

    /**
    *
    * TODO - populate this better, but:
    * This function is used to see if the holdGestData is valid, and if so, update the remaining time
    * and return an appropriate response if the timer expires
    *
    */
    SequenceStatus holdSequenceStatus(sequenceResponse& response);

private:
    SequenceStatus checkLegalRegister(midasMode mode, sequenceInfo seqInfo) const;

    /**
    * Track state information when attempting to start a sequence. Then ensure 
    * that state isn't altered until the end of a sequence, or else something
    * has become corrupt, so cleanup and return an error.
    *
    * @param state The state to verify the associated mode has persisted throughout 
    * an active sequence cycle.
    * @return SequenceStatus associated status information to inform caller of success/lack there of
    */
    SequenceStatus ensureSameState(ControlState state);

    /**
    * Perform the duties of progressSequence, but optimized for the situation where one or more sequences
    * have already been noted as active.
    *
    * @param x Same as progressSequence.
    * @return x Same as progressSequence.
    */
    SequenceStatus progressActiveSequences(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

    /**
    * Perform the duties of progressSequence, but optimized for the situation where no sequence has yet
    * been deemed active.
    *
    * @param x Same as progressSequence.
    * @return x Same as progressSequence.
    */
    SequenceStatus findActivation(myo::Pose::Type gesture, ControlState state, sequenceResponse& response);

    SequenceStatus findRestSeq(ControlState state, sequenceResponse& response);

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

    // This will hold valid data only when there is a hold sequence occuring.
    // Otherwise it will be invalid and therefore unusable.
    HoldGestData holdGestData;
};

#endif /* _GESTURE_SEQ_RECORDER_H */