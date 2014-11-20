#ifndef _GESTURE_FILTER_H
#define _GESTURE_FILTER_H

#include "myo\myo.hpp"
#include "ControlState.h"
#include "Filter.h"

#define GESTURE_INPUT "gesture"
#define MYO_GESTURE_RIGHT_CLICK myo::Pose::fingersSpread
#define MYO_GESTURE_LEFT_CLICK myo::Pose::fist

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
     * Translates gestures into corresponding mouse and keyboard commands.
     *
     * @param pose The pose to translate into a command.
     */
    commandData translateGesture(myo::Pose::Type pose);

    myo::Pose::Type lastPoseType;
    ControlState* controlStateHandle;
    clock_t timeDelta;
    clock_t lastTime;

};

#endif /* _GESTURE_FILTER_H */