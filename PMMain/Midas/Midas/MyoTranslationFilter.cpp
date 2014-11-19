#include "MyoTranslationFilter.h"
#include <cmath>
#include <iostream>

MyoTranslationFilter::MyoTranslationFilter(ControlState* controlState)
    : controlStateHandle(controlState), previousMode(LOCK_MODE), basePitch(0), baseYaw(0)
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

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);

    float pitch = getPitchFromQuaternion(quatX, quatY, quatZ, quatW);
    float yaw = getYawFromQuaternion(quatX, quatY, quatZ, quatW);

    if (previousMode != MOUSE_MODE && controlStateHandle->getMode() == MOUSE_MODE)
    {
        std::cout << "Entering Mouse Mode." << std::endl;
        basePitch = pitch;
        baseYaw = yaw;
    }

    if (controlStateHandle->getMode() != MOUSE_MODE)
    {
        if (previousMode == MOUSE_MODE)
        {
            std::cout << "Leaving Mouse Mode." << std::endl;
            filterDataMap outputToSharedCommandData;
            point mouseUnitVelocity = point(0, 0);
            outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
            Filter::setOutput(outputToSharedCommandData);
        }
        else
        {
            Filter::setFilterStatus(filterStatus::END_CHAIN);
        }
    }
    else
    {
        filterDataMap outputToSharedCommandData;
        point mouseUnitVelocity = getMouseUnitVelocity(pitch, yaw);
        outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;

        Filter::setOutput(outputToSharedCommandData);
    }

    previousMode = controlStateHandle->getMode();
}

point MyoTranslationFilter::getMouseUnitVelocity(float pitch, float yaw)
{
    float deltaPitch = pitch - basePitch;
    float deltaYaw = yaw - baseYaw;

    float unitPitch = (deltaPitch >= 0) ? std::min(1.0f, deltaPitch / MAX_PITCH_ANGLE) : std::max(-1.0f, deltaPitch / MAX_PITCH_ANGLE);
    float unitYaw = (deltaYaw >= 0) ? std::min(1.0f, deltaYaw / MAX_YAW_ANGLE) : std::max(-1.0f, deltaYaw / MAX_YAW_ANGLE);

    return point((int) (unitYaw * 100), (int) (unitPitch * 100));
}

float MyoTranslationFilter::getPitchFromQuaternion(float x, float y, float z, float w)
{
    //TODO - Likely need to base this off of R/L hand, and orientation
    return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
}

float MyoTranslationFilter::getYawFromQuaternion(float x, float y, float z, float w)
{
    //TODO - Likely need to base this off of R/L hand, and orientation
    return -atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
}