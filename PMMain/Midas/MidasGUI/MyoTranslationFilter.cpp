#define NOMINMAX
#include "MyoTranslationFilter.h"
#include <math.h>
#include <iostream>

MyoTranslationFilter::MyoTranslationFilter(ControlState* controlState)
    : controlStateHandle(controlState), previousMode(LOCK_MODE), 
    pitch(0), basePitch(0), prevPitch(0), deltaPitch(0),
    yaw(0), baseYaw(0), prevYaw(0), deltaYaw(0),
    roll(0), baseRoll(0), prevRoll(0), deltaRoll(0)
{
    initGestHoldModeActionArr();
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

    roll = getRollFromQuaternion(quatX, quatY, quatZ, quatW);
    pitch = getPitchFromQuaternion(quatX, quatY, quatZ, quatW, arm, xDirection);
    yaw = getYawFromQuaternion(quatX, quatY, quatZ, quatW);
    int rollDeg = (int)(roll * (180 / M_PI));
    int prevRollDeg;

    midasMode currMode = controlStateHandle->getMode();

    if (previousMode != currMode)
    {
        baseRoll = roll;
        basePitch = pitch;
        baseYaw = yaw;
    }

    deltaRoll = calcRingDelta(roll, baseRoll) - calcRingDelta(prevRoll, baseRoll); // normalized to avoid overflow
    prevRoll = roll;
    prevRollDeg = (int)(prevRoll * (180 / M_PI));
    deltaPitch = calcRingDelta(pitch, basePitch) - calcRingDelta(prevPitch, basePitch); // normalized to avoid overflow
    prevPitch = pitch;
    deltaYaw = calcRingDelta(yaw, baseYaw) - calcRingDelta(prevYaw, baseYaw); // normalized to avoid overflow
    prevYaw = yaw;

    unsigned int gestIdx;
    switch (controlStateHandle->getMode())
    {
    case MOUSE_MODE:
        performMouseModeFunc(outputToSharedCommandData);
        break;
    case GESTURE_HOLD_ONE:
        gestIdx = 0;
    case GESTURE_HOLD_TWO:
        gestIdx = 1;
    case GESTURE_HOLD_THREE:
        gestIdx = 2;
    case GESTURE_HOLD_FOUR:
        gestIdx = 3;
    case GESTURE_HOLD_FIVE:
        gestIdx = 4;

        performHoldModeFunc(gestIdx, outputToSharedCommandData);
        break;
    case KEYBOARD_MODE:
        performeKybdModeFunc(outputToSharedCommandData);
        break;
    default:
        break;
    }

    if (controlStateHandle->getMode() != MOUSE_MODE)
    {
        if (previousMode == MOUSE_MODE)
        {
            point mouseUnitVelocity = point(0, 0);
            outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
        }
    }

/*    ////////////////////////// Trying to port all below into subFunctions
    if (controlStateHandle->getMode() != MOUSE_MODE)
    {
        if (previousMode == MOUSE_MODE)
        {
            point mouseUnitVelocity = point(0, 0);
            outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
        }

        deltaRoll = rollDeg - prevRollDeg;
        prevRollDeg = rollDeg;

        if (controlStateHandle->getMode() == GESTURE_HOLD_THREE)
        {
            // as per GestureFilter, this is executed with a fist, and currently will be tested by changing the volume.
            
            commandData command;
            command.type = commandType::KYBRD_CMD;
            command.name = "Volume Command";
            if (deltaRoll > 0)
            {
                command.action.kybd = kybdCmds::VOLUME_UP;
                outputToSharedCommandData[COMMAND_INPUT] = command;
            }
            else if (deltaRoll < 0)
            {
                
                command.action.kybd = kybdCmds::VOLUME_DOWN;
                outputToSharedCommandData[COMMAND_INPUT] = command;
            }
        } 
        else if (controlStateHandle->getMode() == KEYBOARD_MODE)
        {
            keyboardAngle myoAngle;

            point myoAnglePoint = getMouseUnitVelocity(pitch, yaw);
            unsigned int magnitude = sqrt(pow(myoAnglePoint.x, 2) + pow(myoAnglePoint.y, 2));
            myoAngle.ringThreshReached = false;
            if (magnitude > KEYBOARD_THRESH_MAG)
            {
                myoAngle.ringThreshReached = true;
            }

            // TEMP TODO for debug only
            myoAngle.x = myoAnglePoint.x;
            myoAngle.y = myoAnglePoint.y;

            // TODO - verify/disprove this function 180 - (180.0 / M_PI) * atan2((double)myoAnglePoint.y, (double)myoAnglePoint.x);
            // using 90 instead of 180 *seems* to make it better, but then the upper left quadrant is unnaccessable.
            int myoAngleDegree = 90 - (180.0 / M_PI) * atan2((double)myoAnglePoint.y, (double)myoAnglePoint.x); // NEED to add section size/2 TODO
            while (myoAngleDegree < 0)
            {
                myoAngleDegree += 360;
            }
            
            myoAngle.angle = myoAngleDegree;

            outputToSharedCommandData[ANGLE_INPUT] = myoAngle;
        }
    }
    else
    {
        point mouseUnitVelocity = getMouseUnitVelocity(pitch, yaw);
        outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
    }
    */


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
    if (current >= 2 * M_PI || base >= 2 * M_PI || current < 0 || base < 0)
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
            delta = -((2 * M_PI - current) + base);
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
            delta = (2 * M_PI - base) + current;
        }
    }

    return delta;
}

void MyoTranslationFilter::performHoldModeFunc(unsigned int holdNum, filterDataMap& outputToSharedCommandData)
{
    GestureHoldModeAction currentHoldModeAction = gestHoldModeAction[holdNum];
    
    commandData cd;
    angleData ad;
    bool tryAction = false;

    ad.angleType = angleData::AngleType::ROLL;
    if (deltaRoll > 0)
    {
        tryAction = true;
        ad.anglePositive = true;
        
    } 
    else if (deltaRoll < 0)
    {
        tryAction = true;
        ad.anglePositive = false;
    }
    if (tryAction)
    {
        cd = currentHoldModeAction.getAction(ad);
        if (cd.type != commandType::NONE || cd.type != commandType::UNKNOWN_COMMAND)
        {
            outputToSharedCommandData[COMMAND_INPUT] = cd;
        }
    }

    tryAction = false;
    ad.angleType = angleData::AngleType::PITCH;
    if (deltaPitch > 0)
    {
        tryAction = true;
        ad.anglePositive = true;
    }
    else if (deltaPitch < 0)
    {
        tryAction = true;
        ad.anglePositive = false;
    }
    if (tryAction)
    {
        cd = currentHoldModeAction.getAction(ad);
        if (cd.type != commandType::NONE || cd.type != commandType::UNKNOWN_COMMAND)
        {
            outputToSharedCommandData[COMMAND_INPUT] = cd;
        }
    }

    tryAction = false;
    ad.angleType = angleData::AngleType::YAW;
    if (deltaYaw > 0)
    {
        tryAction = true;
        ad.anglePositive = true;
    }
    else if (deltaYaw < 0)
    {
        tryAction = true;
        ad.anglePositive = false;
    }
    if (tryAction)
    {
        cd = currentHoldModeAction.getAction(ad);
        if (cd.type != commandType::NONE || cd.type != commandType::UNKNOWN_COMMAND)
        {
            outputToSharedCommandData[COMMAND_INPUT] = cd;
        }
    }
}

void MyoTranslationFilter::performMouseModeFunc(filterDataMap& outputToSharedCommandData)
{
    point mouseUnitVelocity = getMouseUnitVelocity(pitch, yaw);
    outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;
}

void MyoTranslationFilter::performeKybdModeFunc(filterDataMap& outputToSharedCommandData)
{
    keyboardAngle myoAngle;

    point myoAnglePoint = getMouseUnitVelocity(pitch, yaw);
    unsigned int magnitude = sqrt(pow(myoAnglePoint.x, 2) + pow(myoAnglePoint.y, 2));
    myoAngle.ringThreshReached = false;
    if (magnitude > KEYBOARD_THRESH_MAG)
    {
        myoAngle.ringThreshReached = true;
    }

    // TEMP TODO for debug only
    myoAngle.x = myoAnglePoint.x;
    myoAngle.y = myoAnglePoint.y;

    // TODO - verify/disprove this function 180 - (180.0 / M_PI) * atan2((double)myoAnglePoint.y, (double)myoAnglePoint.x);
    // using 90 instead of 180 *seems* to make it better, but then the upper left quadrant is unnaccessable.
    int myoAngleDegree = 90 - (180.0 / M_PI) * atan2((double)myoAnglePoint.y, (double)myoAnglePoint.x); // NEED to add section size/2 TODO
    while (myoAngleDegree < 0)
    {
        myoAngleDegree += 360;
    }

    myoAngle.angle = myoAngleDegree;

    outputToSharedCommandData[ANGLE_INPUT] = myoAngle;
}

bool MyoTranslationFilter::initGestHoldModeActionArr(void)
{
    //TODO - use setting defined values for this part. Temporarily hard coded to test concept.

    angleData ad;
    ad.angleType = angleData::AngleType::ROLL;
    ad.anglePositive = true;
    gestHoldModeAction[2].addToActionMap(ad, kybdCmds::VOLUME_UP);
    ad.anglePositive = false;
    gestHoldModeAction[2].addToActionMap(ad, kybdCmds::VOLUME_DOWN);
}