#include "GestureFilter.h"
#include "myo\myo.hpp"
#include <time.h>

GestureFilter::GestureFilter(ControlState* controlState, clock_t timeDel) : timeDelta(timeDel), lastPoseType(myo::Pose::rest),
    lastTime(0), controlStateHandle(controlState)
{
}

GestureFilter::~GestureFilter()
{
}

void GestureFilter::process()
{
    filterDataMap input = Filter::getInput();
    myo::Pose::Type gesture = boost::any_cast<myo::Pose::Type>(input[GESTURE_INPUT]);
    
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);

    if (gesture != lastPoseType)
    {
        // The user's gesture has changed.
        if (gesture == myo::Pose::rest)
        {
            filterDataMap outputToSharedCommandData;
            commandData command;
            command.type = MOUSE_COMMAND;

            if (lastPoseType == myo::Pose::fist)
            {
                command.mouse = LEFT_RELEASE;
                outputToSharedCommandData[COMMAND_INPUT] = command;
                Filter::setOutput(outputToSharedCommandData);
                return;
            }
            else if (lastPoseType == myo::Pose::fingersSpread)
            {
                command.mouse = RIGHT_RELEASE;
                outputToSharedCommandData[COMMAND_INPUT] = command;
                Filter::setOutput(outputToSharedCommandData);
                return;
            }
        }

        lastPoseType = gesture;
        lastTime = clock();
    }

    if (clock() - lastTime >= timeDelta)
    {
        // The user has held the same gesture for a long enough
        // period of time.
        if (gesture == myo::Pose::thumbToPinky)
        {
            // Lock <--> Unlock gesture.
            midasMode currentState = controlStateHandle->getMode();

            if (currentState == LOCK_MODE)
            {
                controlStateHandle->setMode(MOUSE_MODE);
            }
            else
            {
                controlStateHandle->setMode(LOCK_MODE);
            }

            Filter::setFilterStatus(filterStatus::END_CHAIN);
        }
        else
        {
            // Not the state-change pose
            filterDataMap outputToSharedCommandData;
            commandData sendData = translateGesture(gesture);

            if (sendData.type == UNKNOWN_COMMAND)
            {
                Filter::setFilterStatus(filterStatus::END_CHAIN);
            }
            else
            {
                outputToSharedCommandData[COMMAND_INPUT] = sendData;
                Filter::setOutput(outputToSharedCommandData);
            }
        }
    }
}

commandData GestureFilter::translateGesture(myo::Pose::Type pose)
{
    commandData command;
    command.type = MOUSE_COMMAND;

    if (pose == myo::Pose::fist)
    {
        command.mouse = LEFT_CLICK;
    }
    else if (pose == myo::Pose::fingersSpread)
    {
        command.mouse = RIGHT_CLICK;
    }
    else
    {
        command.type = UNKNOWN_COMMAND;
    }

    return command;
}