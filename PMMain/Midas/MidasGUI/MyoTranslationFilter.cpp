#define NOMINMAX
#include "MyoTranslationFilter.h"
#include <math.h>
#include <iostream>

MyoTranslationFilter::MyoTranslationFilter(ControlState* controlState)
    : controlStateHandle(controlState), previousMode(LOCK_MODE), basePitch(0), baseYaw(0), prevRoll(0), deltaRoll(0)
{
}

MyoTranslationFilter::~MyoTranslationFilter()
{
}

void MyoTranslationFilter::process()
{
    filterDataMap input = Filter::getInput();
    float quatX = boost::any_cast<float>(input[INPUT_QUATERNION_X]);
    float quatY = boost::any_cast<float>(input[INPUT_QUATERNION_Y]);
    float quatZ = boost::any_cast<float>(input[INPUT_QUATERNION_Z]);
    float quatW = boost::any_cast<float>(input[INPUT_QUATERNION_W]);
    filterDataMap outputToSharedCommandData;

    Arm arm = boost::any_cast<Arm>(input[INPUT_ARM]);
    XDirection xDirection = boost::any_cast<XDirection>(input[INPUT_X_DIRECTION]);

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);

    float pitch = getPitchFromQuaternion(quatX, quatY, quatZ, quatW, arm, xDirection);
    float yaw = getYawFromQuaternion(quatX, quatY, quatZ, quatW);
    int rollDeg = (int)(getRollFromQuaternion(quatX, quatY, quatZ, quatW) * (180 / M_PI));

    if (previousMode != MOUSE_MODE && controlStateHandle->getMode() == MOUSE_MODE)
    {
        basePitch = pitch;
        baseYaw = yaw;
    }

    outputToSharedCommandData[DELTA_VOL] = 0;
    if (controlStateHandle->getMode() != MOUSE_MODE)
    {
        if (previousMode == MOUSE_MODE)
        {
            point mouseUnitVelocity = point(0, 0);
            outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
        }    

        // Jorden TODO -- put other "if (controlStateHandle->getMode() == XXX)" statements here, and generate output
        // to populate the sharedcommanddata with. Such as rotational data for volume
        // temp
        if (controlStateHandle->getMode() == GESTURE_HOLD_THREE)
        {
            // as per GestureFilter, this is executed with a fist, and currently will be tested by changing the volume.
            deltaRoll = rollDeg - prevRoll;
            prevRoll = rollDeg;
            outputToSharedCommandData[DELTA_VOL] = deltaRoll;    
        }
    }
    else
    {
        point mouseUnitVelocity = getMouseUnitVelocity(pitch, yaw);
        outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
    }
    Filter::setOutput(outputToSharedCommandData);

    previousMode = controlStateHandle->getMode();
}

point MyoTranslationFilter::getMouseUnitVelocity(float pitch, float yaw)
{
    // Data is on range -180 to +180. convert to 0-360.
    float deltaPitch = calcRingDelta(pitch + M_PI, basePitch + M_PI);
    float deltaYaw = calcRingDelta(yaw + M_PI, baseYaw + M_PI); 

    float unitPitch = (deltaPitch >= 0) ? std::min(1.0f, deltaPitch / MAX_PITCH_ANGLE) : std::max(-1.0f, deltaPitch / MAX_PITCH_ANGLE);
    float unitYaw = (deltaYaw >= 0) ? std::min(1.0f, deltaYaw / MAX_YAW_ANGLE) : std::max(-1.0f, deltaYaw / MAX_YAW_ANGLE);

    return point((int) (unitYaw * 100), (int) (unitPitch * 100));
}

float MyoTranslationFilter::getPitchFromQuaternion(float x, float y, float z, float w, Arm arm, XDirection xDirection)
{
    if (arm == Arm::armLeft && xDirection == XDirection::xDirectionTowardElbow)
    {
        return asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
    else if (arm == Arm::armRight && xDirection == XDirection::xDirectionTowardElbow)
    {
        return asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
    else if (arm == Arm::armLeft && xDirection == XDirection::xDirectionTowardWrist)
    {
        return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
    else if (arm == Arm::armRight && xDirection == XDirection::xDirectionTowardWrist)
    {
        return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    } 
    else
    {
        // Default - arbitrarily chosen.
        return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
}

float MyoTranslationFilter::getYawFromQuaternion(float x, float y, float z, float w)
{
    // Indifferent to arm/xDirection, unlike pitch.
    return -atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
}

float MyoTranslationFilter::getRollFromQuaternion(float x, float y, float z, float w)
{
    return atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));
}

float MyoTranslationFilter::calcRingDelta(float current, float base)
{
    // Assert angles are within range of a circle [0, 2Pi)
    if (current >= 2 * M_PI || base >= 2 * M_PI || current <  0 || base < 0)
    {
        return 0.0;
    }

    float delta = 0.0;
    if (current >= base)
    {
        if (current - base <= M_PI)
        {
            delta = current - base;
        }
        else
        {
            delta = -((2*M_PI - current) + base);
        }
    }
    else
    {
        if (base - current <= M_PI)
        {
            delta = current - base;
        }
        else
        {
            delta = (2*M_PI - base) + current;
        }
    }

    return delta;
}