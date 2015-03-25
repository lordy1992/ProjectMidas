#ifndef _GESTURE_FILTER_H
#define _GESTURE_FILTER_H

#include "ControlState.h"
#include "Filter.h"
#include "GestureSeqRecorder.h"
#include "MainGUI.h"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
#include "myo\myo.hpp"
using namespace myo;
#endif

#define GESTURE_INPUT "gesture"
#define MYO_GESTURE_RIGHT_MOUSE Pose::fingersSpread
#define MYO_GESTURE_LEFT_MOUSE Pose::fist

// GestureFilter spawns a thread to execute callback functions at this
// period.
#define SLEEP_LEN 20 // ms



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
    GestureFilter(ControlState* controlState, clock_t timeDel, MainGUI *mainGuiHandle);
    ~GestureFilter();

    /**
     * This is the process() function that overrides the base Filter class process(). It handles 
     * the logic of the filter. It translates gestures to the corresponding mouse and keyboard commands,
     * handles changing the application state, and enforces the timing restriction that prevents, or rather
     * reduces the chance of, false positive gestures from being recognized.
     */
    void process();

    filterDataMap getExtraDataForSCD();

    filterError updateBasedOnProfile(ProfileManager& pm, std::string name);

    /**
    * return actual handle to gestSeqRecorder
    */
    GestureSeqRecorder *getGestureSeqRecorder() { return gestSeqRecorder; }

    static void handleStateChange(commandData response);

    friend void setupCallbackThread(GestureFilter *gf);
    friend void callbackThreadWrapper(GestureFilter *gf);

private:

    filterDataMap extraDataForSCD;

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
    void handleMouseCommand(commandData response);
    void handleKybrdCommand(commandData response, bool addToExtra = false);

    void emitPoseData(int poseInt);

    Pose::Type lastPoseType;
    
    static ControlState* controlStateHandle;
    clock_t timeDelta;
    clock_t lastTime;

    GestureSeqRecorder* gestSeqRecorder;

    MainGUI *mainGui;
    static GestureSignaller signaller;

    SequenceImageManager imageManager;
};

#endif /* _GESTURE_FILTER_H */