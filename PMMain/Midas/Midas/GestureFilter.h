#ifndef _GESTURE_FILTER_H
#define _GESTURE_FILTER_H

#include "myo\myo.hpp"
#include "ControlState.h"
#include "Filter.h"
#include "GestureSeqRecorder.h"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#endif

#ifdef USE_SIMULATOR
using namespace myoSim;
#else
using namespace myo;
#endif

#define GESTURE_INPUT "gesture"
#define MYO_GESTURE_RIGHT_MOUSE Pose::fingersSpread
#define MYO_GESTURE_LEFT_MOUSE Pose::fist
#define DEFAULT_PROG_MAX_DELTA 1000 // ms

/**
* An enumeration of all possible sequences that can be executed from various
* states. This MUST be in binary values, as more than sequence could be active.
* This allows for sequences to vary by only their final value, and still be valid.
*/
enum class activeSequence
{
    NONE = 1,
    UNLOCK = 2,
    LOCK = 4,
    MOUSE_TO_GEST = 8,
    GEST_TO_MOUSE = 16,
    MOUSE_TO_KYBRD = 32,
    KYBRD_TO_MOUSE = 64
};

/**
* Overloading logical OR, so that activeSequence can be used as a mask, such that
* more than one sequence can be started, and they can all be uniquely identified!
*/
inline activeSequence operator|(activeSequence a, activeSequence b)
{
    return static_cast<activeSequence>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

/**
* Overloading logical AND, so that activeSequence can be used as a mask, such that
* more than one sequence can be started, and they can all be uniquely identified!
*/
inline activeSequence operator&(activeSequence a, activeSequence b)
{
    return static_cast<activeSequence>(static_cast<unsigned int>(a)& static_cast<unsigned int>(b));
}

/**
* Overloading logical NOT, so that activeSequence can be used as a mask, such that
* more than one sequence can be started, and they can all be uniquely identified!
*/
inline activeSequence operator~(activeSequence a)
{
    return static_cast<activeSequence>(~static_cast<unsigned int>(a));
}

/**
 * Consult Filter.h for concepts regarding Filters.
 * 
 * A filter specific to the Myo armband that handles changing the application
 * state when a certain gesture is performed, and translates from Myo gestures
 * to mouse and keyboard commands. Furthermore, helps to prevent against false 
 * positive gesture recognition by forcing the user to hold a gesture for a
 * certain amount of time before registering it.
 */
class GestureFilter : public Filter
{
public:
    /**
     * The constructor for the GestureFilter. It takes a ControlState handle so
     * that the state of the application can be changed by gesture control. It sets
     * the time that must occur before a gesture is registered.
     *
     * @param controlState A handle to the ControlState object that manages application state.
     * @param timeDel The time that a user must hold a gesture before it is registered.
     */
    GestureFilter(ControlState* controlState, clock_t timeDel);
    ~GestureFilter();

    /**
     * This is the process() function that overrides the base Filter class process(). It handles 
     * the logic of the filter. It translates gestures to the corresponding mouse and keyboard commands,
     * handles changing the application state, and enforces the timing restriction that prevents, or rather
     * reduces the chance of, false positive gestures from being recognized.
     */
    void process();

private:

    /**
    * Helper class that handles the state transitions of Midas, based on gesture input.
    * It looks at what gestures the user is supplying, in what order, and how frequently,
    * in order to determine when and how state changes should occur. The state changing
    * sequences can be found in the sequence vector member variables, initialized in 
    * constructor.
    */
    class StateHandler
    {
    public:
        /**
        * The constructor for the StateHandler. It takes a reference to its parent,
        * so that it can call it's parent's functions.
        *
        * @param parent A handle to the parent of the StateHandler - a GestureFilter
        */
        StateHandler(GestureFilter& parent);
        ~StateHandler();

        /**
        * Update the state of Midas, given a recieved gesture.
        * 
        * @param gesture This is the current recorded gesture being used to determine if the state 
        * will change.
        * @return Returns true if the state has been changed, and false otherwise.
        */
        bool updateState(Pose::Type gesture);

        /**
        * Modifier.
        *
        * @param newTime The new stateProgressMaxDeltaTime.
        */
        void setStateProgressMaxDeltaTime(clock_t newTime);

        /**
        * Accessor.
        *
        * @return Value of stateProgressMaxDeltaTime.
        */
        clock_t getStateProgressMaxDeltaTime(void);

    private:
        bool GestureFilter::StateHandler::satisfyStateChange(activeSequence desiredSeq, std::vector<Pose::Type> sequence,
            Pose::Type gesture, midasMode desiredMode, midasMode& nextMode, bool& willTransition);

        void GestureFilter::StateHandler::checkProgressInSequence(activeSequence desiredSeq, std::vector<Pose::Type> sequence,
            Pose::Type gesture, bool& progressSeq, clock_t now);

        GestureFilter& parent;

        // Gesture sequences required to change states
        std::vector<Pose::Type> unlockSequence;
        std::vector<Pose::Type> lockSequence;
        std::vector<Pose::Type> mouseToGestureSequence;
        std::vector<Pose::Type> gestureToMouseSequence;
        std::vector<Pose::Type> mouseToKeyboardSequence;
        std::vector<Pose::Type> keyboardToMouseSequence;

        // Gesture sequence completion value - represents how far into a gesture sequence
        // the user is. Ranges from 0 to max(sequence length - 1) of allowable sequences from
        // any given state.
        unsigned int sequenceCount;

        // Indicates the sequence which is actively being attempted to be completed.
        activeSequence activeSeq;

        // Base timestamp used to calculate transitions
        clock_t stateProgressBaseTime;

        // Transition timeout value. For example, if to change a state,
        // the user is required to perform "wave in, wave out, pinky-to-thumb", 
        // then when a wave in is recorded, the user has up to "stateTransMaxDeltaTime"
        // milliseconds to perform a wave out, or else the whole process needs to be repeated.
        clock_t stateProgressMaxDeltaTime;
    };


    /**
     * Translates gestures into corresponding mouse and keyboard commands.
     *
     * @param pose The pose to translate into a command.
     */
    commandData translateGesture(Pose::Type pose);

    // registration functions. to be commented after integration success.
    void registerMouseSequences(void);
    void registerKeyboardSequences(void);
    void registerStateSequences(void);
    void handleStateChange(sequenceResponse response);

    Pose::Type lastPoseType;
    ControlState* controlStateHandle;
    clock_t timeDelta;
    clock_t lastTime;
    StateHandler stateHandler;

    GestureSeqRecorder gestSeqRecorder;
};

#endif /* _GESTURE_FILTER_H */