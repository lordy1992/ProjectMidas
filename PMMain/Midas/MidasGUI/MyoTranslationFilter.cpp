#define NOMINMAX
#include "MyoTranslationFilter.h"
#include "ProfileManager.h"
#include <math.h>
#include <iostream>

float radToDeg(float rad)
{
    return rad * (180.0 / M_PI);
}

MyoTranslationFilter::MyoTranslationFilter(ControlState* controlState)
    : controlStateHandle(controlState), previousMode(LOCK_MODE), 
    pitch(0), basePitch(0), prevPitch(0), deltaPitchDeg(0),
    yaw(0), baseYaw(0), prevYaw(0), deltaYawDeg(0),
    roll(0), baseRoll(0), prevRoll(0), deltaRollDeg(0)
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

    deltaRollDeg = radToDeg(calcRingDelta(roll, baseRoll) - calcRingDelta(prevRoll, baseRoll)); // normalized to avoid overflow
    prevRoll = roll;
    prevRollDeg = (int)(prevRoll * (180 / M_PI));
    deltaPitchDeg = radToDeg(calcRingDelta(pitch, basePitch) - calcRingDelta(prevPitch, basePitch)); // normalized to avoid overflow
    prevPitch = pitch;
    deltaYawDeg = radToDeg(calcRingDelta(yaw, baseYaw) - calcRingDelta(prevYaw, baseYaw)); // normalized to avoid overflow
    prevYaw = yaw;

    unsigned int gestIdx;
    switch (controlStateHandle->getMode())
    {
    case MOUSE_MODE:
        performMouseModeFunc(outputToSharedCommandData);
        break;
    case GESTURE_HOLD_ONE:
        gestIdx = 0;
        goto execute;
    case GESTURE_HOLD_TWO:
        gestIdx = 1;
        goto execute;
    case GESTURE_HOLD_THREE:
        gestIdx = 2;
        goto execute;
    case GESTURE_HOLD_FOUR:
        gestIdx = 3;
        goto execute;
    case GESTURE_HOLD_FIVE:
        gestIdx = 4;

    execute:
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

    Filter::setOutput(outputToSharedCommandData);

    previousMode = controlStateHandle->getMode();
}

point MyoTranslationFilter::getMouseUnitVelocity(float pitch, float yaw)
{
    float deltaPitch = calcRingDelta(pitch, basePitch);
    float deltaYaw = calcRingDelta(yaw, baseYaw); 

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
    // Assert angles are within range of a circle [Pi, Pi)
    if (current >= M_PI || base >= M_PI || current < -M_PI || base < -M_PI)
    {
        return 0.0;
    }
    
    // normalize to 0 - 2Pi for this upcoming calculation.
    current += M_PI;
    base += M_PI;

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
    commandData command;
    command.type = commandType::KYBRD_CMD;
    command.name = "HoldMode Command";
    command.action.kybd = kybdCmds::NO_CMD;

    GestureHoldModeAction currentHoldModeAction = gestHoldModeAction[holdNum];
    float thresh = .1;
    
    commandData cd;
    angleData ad;
    bool tryAction = false;

    ad.angleType = angleData::AngleType::ROLL;
    if (deltaRollDeg > thresh)
    {
        tryAction = true;
        ad.anglePositive = true;
        
    } 
    else if (deltaRollDeg < -thresh)
    {
        tryAction = true;
        ad.anglePositive = false;
    }
    if (tryAction)
    {
        command.action.kybd = kybdCmds((unsigned int)command.action.kybd | (unsigned int)currentHoldModeAction.getAction(ad));
        outputToSharedCommandData[COMMAND_INPUT] = command;
    }

    tryAction = false;
    ad.angleType = angleData::AngleType::PITCH;
    if (deltaPitchDeg > thresh)
    {
        tryAction = true;
        ad.anglePositive = true;
    }
    else if (deltaPitchDeg < -thresh)
    {
        tryAction = true;
        ad.anglePositive = false;
    }
    if (tryAction)
    {
        command.action.kybd = kybdCmds((unsigned int)command.action.kybd | (unsigned int)currentHoldModeAction.getAction(ad));
        outputToSharedCommandData[COMMAND_INPUT] = command;
    }

    tryAction = false;
    ad.angleType = angleData::AngleType::YAW;
    if (deltaYawDeg > thresh)
    {
        tryAction = true;
        ad.anglePositive = true;
    }
    else if (deltaYawDeg < -thresh)
    {
        tryAction = true;
        ad.anglePositive = false;
    }
    if (tryAction)
    {
        command.action.kybd = kybdCmds((unsigned int)command.action.kybd | (unsigned int)currentHoldModeAction.getAction(ad));
        outputToSharedCommandData[COMMAND_INPUT] = command;
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

    // Use ProfileManager here.

    bool initOkay = true;

    angleData ad;
    ad.angleType = angleData::AngleType::ROLL;
    ad.anglePositive = true;
    initOkay &= gestHoldModeAction[GESTURE_FIST].addToActionMap(ad, kybdCmds::VOLUME_UP);
    ad.anglePositive = false;
    initOkay &= gestHoldModeAction[GESTURE_FIST].addToActionMap(ad, kybdCmds::VOLUME_DOWN);

    ad.angleType = angleData::AngleType::PITCH;
    ad.anglePositive = true;
    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::UP_ARROW);
    ad.anglePositive = false;
    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::DOWN_ARROW);

    ad.angleType = angleData::AngleType::YAW;
    ad.anglePositive = true;
    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::RIGHT_ARROW);
    ad.anglePositive = false;
    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::LEFT_ARROW);

    ad.angleType = angleData::AngleType::PITCH;
    ad.anglePositive = true;
    initOkay &= gestHoldModeAction[GESTURE_THUMB_TO_PINKY].addToActionMap(ad, kybdCmds::ZOOM_IN);
    ad.anglePositive = false;
    initOkay &= gestHoldModeAction[GESTURE_THUMB_TO_PINKY].addToActionMap(ad, kybdCmds::ZOOM_OUT);

    ad.angleType = angleData::AngleType::YAW;
    ad.anglePositive = false;
    initOkay &= gestHoldModeAction[GESTURE_WAVE_IN].addToActionMap(ad, kybdCmds::UNDO);
    ad.anglePositive = true;
    initOkay &= gestHoldModeAction[GESTURE_WAVE_OUT].addToActionMap(ad, kybdCmds::REDO);

    return initOkay;
}

void MyoTranslationFilter::unregisterHoldModeActions(void)
{
    for (int i = 0; i < NUM_GESTURES; i++)
    {
        gestHoldModeAction[i].clearMap();
    }
}

void MyoTranslationFilter::dynRegHoldModeActionArr(void)
{
    this->unregisterHoldModeActions();

    ProfileManager pm;
    pm.loadProfilesFromFile("TODO - THIS NEEDS A CONSTANT FILE NAME");
    std::vector<profile>* profiles = pm.getProfiles();

    bool okay = true;
    angleData ad;
    for (std::vector<profile>::iterator it = profiles->begin(); it != profiles->end(); ++it)
    {
        ad.angleType = angleData::AngleType::ROLL; // TODO make dynamic from it
        ad.anglePositive = true; //TODO make dynamic from it
        okay &= gestHoldModeAction[GESTURE_FIST].addToActionMap(ad, kybdCmds::VOLUME_UP); //TODO make dynamic from it
        
        if (okay = false)
        {
            throw new std::exception("registerHoldModeActionException: XXXSEQUENCENAME");
        }
    }
}
