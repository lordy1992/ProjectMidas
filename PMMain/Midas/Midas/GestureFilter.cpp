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

    // mouseToGestureSequence.push_back(myo::Pose::fingersSpread);
    // gestureToMouseSequence.push_back(myo::Pose::fingersSpread);
    // mouseToKeyboardSequence.push_back(myo::Pose::fingersSpread);
    // keyboardToMouseSequence.push_back(myo::Pose::fingersSpread);

    sequenceCount = 0;
    activeSeq = activeSequence::NONE;
    restBetweenPoses = false;
}

GestureFilter::StateHandler::~StateHandler()
{
}

bool GestureFilter::StateHandler::updateState(myo::Pose::Type gesture)
{
    if (gesture == myo::Pose::Type::rest)
    {
        restBetweenPoses = true;
    }

    midasMode currentState = parent.controlStateHandle->getMode();
    
    bool didTransition = false;
    clock_t now = clock();

    if (activeSeq != activeSequence::NONE)
    {
        if (restBetweenPoses == false)
        {
            // Early exit until a user has rested between poses. This stops states from 
            // jittering back and forth if they have the same gesture to enter/exit.
            return false;
        }

        if (now - stateProgressBaseTime > stateProgressMaxDeltaTime)
        {
            // reset and return if a sequence timed out.
            activeSeq = activeSequence::NONE;
            sequenceCount = 0;
            return false;
        }
    }

    if (currentState == LOCK_MODE)
    {
        // Can only unlock from this locked state
        if (activeSeq == activeSequence::UNLOCK)
        {
            if (gesture == unlockSequence.at(sequenceCount))
            {
                sequenceCount++;
                stateProgressBaseTime = now;
            }
        }
        else
        {
            // Attempting to start a legal sequence.
            // Only one possible sequence to get out of locked state.
            if (gesture = unlockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::UNLOCK;
                sequenceCount++;
                stateProgressBaseTime = now;
            }
            else
            {
                // Failed to activate a sequence
                sequenceCount = 0;
            }
        }

        if (sequenceCount == unlockSequence.size())
        {
            // Succeeded in completing unlock sequence
            parent.controlStateHandle->setMode(MOUSE_MODE);
            didTransition = true;
        }
    }
    else if (currentState == MOUSE_MODE)
    {
        if (activeSeq == activeSequence::LOCK && gesture == lockSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else if (activeSeq == activeSequence::MOUSE_TO_GEST && gesture == mouseToGestureSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else if (activeSeq == activeSequence::MOUSE_TO_KYBRD && gesture == mouseToKeyboardSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else
        {
            // Attempting to start a legal sequence.
            sequenceCount++;
            stateProgressBaseTime = now;
            if (gesture = lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::LOCK;   
            }
            else if (gesture = mouseToGestureSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::MOUSE_TO_GEST;
            }
            else if (gesture = mouseToKeyboardSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::MOUSE_TO_KYBRD;
            }
            else
            {
                // Failed to activate a sequence
                sequenceCount = 0;
            }

        }

        if (activeSeq == activeSequence::LOCK && sequenceCount == lockSequence.size())
        {
            // Succeeded in completing lock sequence
            parent.controlStateHandle->setMode(LOCK_MODE);
            didTransition = true;
        }
        else if (activeSeq == activeSequence::MOUSE_TO_GEST && sequenceCount == mouseToGestureSequence.size())
        {
            // Succeeded in completing mouse to gesture sequence
            parent.controlStateHandle->setMode(GESTURE_MODE);
            didTransition = true;
        }
        else if (activeSeq == activeSequence::MOUSE_TO_KYBRD && sequenceCount == mouseToKeyboardSequence.size())
        {
            // Succeeded in completing mouse to keyboard sequence
            parent.controlStateHandle->setMode(KEYBOARD_MODE);
            didTransition = true;
        }
    }
    else if (currentState == GESTURE_MODE)
    {
        //TODO
    }
    else if (currentState == KEYBOARD_MODE)
    {
        //TODO
    }

    // Reset if succeeded in transition
    if (didTransition)
    {
        activeSeq = activeSequence::NONE;
        sequenceCount = 0;
        restBetweenPoses = false;
        return true;
    }

    return false;

    // --------------- OLD , yet functional logic.
        
 //   if (gesture == myo::Pose::thumbToPinky)
 //   {
 //       // Lock <--> Unlock gesture.
 //       if (currentState == LOCK_MODE)
 //       {
 //           parent.controlStateHandle->setMode(MOUSE_MODE);
 //       }
 //       else
 //       {
 //           parent.controlStateHandle->setMode(LOCK_MODE);
 //       }
 //
 //       return true;
 //   }
 //   else
 //   {
 //       // Not the state-change pose
 //       return false;
 //   }
 //
 //   // Return default that state has not changed.
 //   return false;
}