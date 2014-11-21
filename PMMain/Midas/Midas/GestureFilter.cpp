#include "GestureFilter.h"
#include "myo\myo.hpp"
#include <time.h>

GestureFilter::GestureFilter(ControlState* controlState, clock_t timeDel) : timeDelta(timeDel), lastPoseType(myo::Pose::rest),
    lastTime(0), controlStateHandle(controlState), stateHandler(*this)
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

            if (lastPoseType == MYO_GESTURE_LEFT_MOUSE)
            {
                command.mouse = LEFT_RELEASE;
                outputToSharedCommandData[COMMAND_INPUT] = command;
                Filter::setOutput(outputToSharedCommandData);
                return;
            }
            else if (lastPoseType == MYO_GESTURE_RIGHT_MOUSE)
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

        if (stateHandler.updateState(gesture))
        {
            // State changed. Alert pipeline that filter chain is done.
            Filter::setFilterStatus(filterStatus::END_CHAIN);
        }
        else
        {
            // No state change. Pass data along pipeline
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

    if (pose == MYO_GESTURE_LEFT_MOUSE)
    {
        command.mouse = LEFT_CLICK;
    }
    else if (pose == MYO_GESTURE_RIGHT_MOUSE)
    {
        command.mouse = RIGHT_CLICK;
    }
    else
    {
        command.type = UNKNOWN_COMMAND;
    }

    return command;
}

GestureFilter::StateHandler::StateHandler(GestureFilter& parent) : parent(parent)
{
    unlockSequence.push_back(myo::Pose::thumbToPinky);
    lockSequence.push_back(myo::Pose::thumbToPinky);

    // None of the following modes actually have functionality, so their 
    // state transition sequences are arbitrary and incomplete. TODO.
    mouseToGestureSequence.push_back(myo::Pose::fingersSpread);
    gestureToMouseSequence.push_back(myo::Pose::fingersSpread);
    mouseToKeyboardSequence.push_back(myo::Pose::fingersSpread);
    keyboardToMouseSequence.push_back(myo::Pose::fingersSpread);

    sequenceCount = 0;
    activeSeq = activeSequence::None;
}

GestureFilter::StateHandler::~StateHandler()
{
}

bool GestureFilter::StateHandler::updateState(myo::Pose::Type gesture)
{
    midasMode currentState = parent.controlStateHandle->getMode();
    
    // determine which gesture must be completed to progress through a sequence.
    myo::Pose::Type gestureToProgress = myo::Pose::Type::rest;

    if (currentState == LOCK_MODE)
    {
        // Can only unlock from this state
        if (activeSeq == activeSequence::UNLOCK)
        {
            gestureToProgress = unlockSequence.at(sequenceCount);
        }
        else
        {

        }
    }
    else if (currentState == MOUSE_MODE)
    {

    }
    else if (currentState == GESTURE_MODE)
    {

    }
    else if (currentState == KEYBOARD_MODE)
    {

    }
        
    if (gesture == myo::Pose::thumbToPinky)
    {
        // Lock <--> Unlock gesture.
        if (currentState == LOCK_MODE)
        {
            parent.controlStateHandle->setMode(MOUSE_MODE);
        }
        else
        {
            parent.controlStateHandle->setMode(LOCK_MODE);
        }

        return true;
    }
    else
    {
        // Not the state-change pose
        return false;
    }

    // Return default that state has not changed.
    return false;
}