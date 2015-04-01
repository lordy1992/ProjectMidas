#ifndef _MYO_TRANSLATION_FILTER_H
#define _MYO_TRANSLATION_FILTER_H

#include "Filter.h"
#include "ControlState.h"
#include "GestureHoldModeAction.h"
#include "ProfileManager.h"
#include "myo\myo.hpp"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

#define INPUT_QUATERNION_X "quatDataX"
#define INPUT_QUATERNION_Y "quatDataY"
#define INPUT_QUATERNION_Z "quatDataZ"
#define INPUT_QUATERNION_W "quatDataW"
#define INPUT_ARM "arm"
#define INPUT_X_DIRECTION "xDirection"

// Testing Constants -- Modify until seems reasonable
#define MAX_PITCH_ANGLE 0.7853981634f //45deg. //1.04719755f //60 deg /* Maximum delta angle in radians */
#define MAX_YAW_ANGLE 0.7853981634f //45deg. //1.04719755f //60 deg /* Maximum delta angle in radians */

#define KEYBOARD_THRESH_MAG 30

#define NUM_GESTURES 5
#define GESTURE_DOUBLE_TAP 0 // used as indexes into gestHoldModeAction
#define GESTURE_FINGERS_SPREAD 1
#define GESTURE_FIST 2
#define GESTURE_WAVE_IN 3
#define GESTURE_WAVE_OUT 4

/**
 * Consult Filter.h for concepts regarding Filters.
 * Handles translating from Myo orientation data to mouse movement information.
 */
class MyoTranslationFilter : public Filter
{
public:
    /**
     * The constructor for MyoTranslationFilter.
     *
     * @param controlState A handle to ControlState, to keep track of the application state.
     */
    MyoTranslationFilter(ControlState* controlState);
    ~MyoTranslationFilter();

    /**
     * Convert the supplied orientation data into mouse movement information, in the form of a
     * percent of the total mouse velocity along both the x and y axes. 
     */
    void process();

    /**
    * Calculates the delta (in radians) between a base angle and a
    * current angle, with respect to a ring. The pupose is to ensure that wrapping
    * around the crossover section of the ring has no effect on the output
    * which should safely range from -pi to +pi.
    *
    * @param current The current angle (in radians) that is being compared (from -Pi to pi rad)
    * @param base The base angle (in radians) that is being compared against
    * @return a value from -pi to +pi representing the delta between two input angles
    */
    static float calcRingDelta(float current, float base);

    filterError updateBasedOnProfile(ProfileManager& pm, std::string name);

private:
    /**
     * Calculate the 'pitch' angle from the supplied quaternion, consisting of x, y, z and w,
     * and infer sign based on arm and xDirection.
     *
     * @param x The x coordinate of the quaternion.
     * @param y The y coordinate of the quaternion.
     * @param z The z coordinate of the quaternion.
     * @param w The w coordinate of the quaternion.
     * @param arm The arm of the user, which is accounted for by sign of calculated Pitch.
     * @param xDirection The xDirection of the user, which is accounted for by sign of calculated Pitch.
     * @return Returns the 'pitch' angle, in radians.
     */
    float getPitchFromQuaternion(float x, float y, float z, float w, Arm arm, XDirection xDirection);

    /**
    * Calculate the 'yaw' angle from the supplied quaternion, consisting of x, y, z and w.
    * Unlike pitch, yaw is indifferent to arm/xDirection of Myo.
    *
    * @param x The x coordinate of the quaternion.
    * @param y The y coordinate of the quaternion.
    * @param z The z coordinate of the quaternion.
    * @param w The w coordinate of the quaternion.
    * @return Returns the 'yaw' angle, in radians.
    */
    float getYawFromQuaternion(float x, float y, float z, float w);

    /**
    * Calculate the 'roll' angle from the supplied quaternion, consisting of x, y, z and w.
    * Unlike pitch, roll is indifferent to arm/xDirection of Myo.
    *
    * @param x The x coordinate of the quaternion.
    * @param y The y coordinate of the quaternion.
    * @param z The z coordinate of the quaternion.
    * @param w The w coordinate of the quaternion.
    * @return Returns the 'roll' angle, in radians.
    */
    float getRollFromQuaternion(float x, float y, float z, float w);

    /**
    * Calculates the percent of the mouse cursor's total velocity along the x
    * and y axis, in the form of a point.
    *
    * @param pitch The pitch angle, in radians.
    * @param yaw The yaw angle, in radians.
    * @return A point with the x coordinate being the percent of the velocity along the x axis, 
    * and y being the percent of the total velocity along the y axis.
    */
    point getMouseUnitVelocity(float pitch, float yaw);

    void performHoldModeFunc(unsigned int holdNum, filterDataMap& outputToSharedCommandData);
    void performMouseModeFunc(filterDataMap& outputToSharedCommandData);
    void performeKybdModeFunc(filterDataMap& outputToSharedCommandData);

    bool initGestHoldModeActionArr(void);
    void unregisterHoldModeActions(void);

    ControlState* controlStateHandle;
    midasMode previousMode;
    float pitch, basePitch, prevPitch, deltaPitchDeg, 
        yaw, baseYaw, prevYaw, deltaYawDeg,
        roll, baseRoll, prevRoll, deltaRollDeg;

    GestureHoldModeAction gestHoldModeAction[5];
};

#endif