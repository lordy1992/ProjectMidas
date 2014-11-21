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
        if (gesture == myo::Pose::rest && controlStateHandle->getMode() != midasMode::LOCK_MODE)
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
            if (controlStateHandle->getMode() != midasMode::LOCK_MODE) // TODO - make this work as desired!
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
            else
            {
                Filter::setFilterStatus(filterStatus::END_CHAIN);
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
    unlockSequence.push_back(myo::Pose::waveIn);
    unlockSequence.push_back(myo::Pose::waveOut);

    lockSequence.push_back(myo::Pose::thumbToPinky);
    lockSequence.push_back(myo::Pose::waveIn);

    // None of the following modes actually have functionality, so their 
    // state transition sequences are arbitrary and incomplete. TODO.
    mouseToGestureSequence.push_back(myo::Pose::fingersSpread);
    mouseToGestureSequence.push_back(myo::Pose::thumbToPinky);
    
    gestureToMouseSequence.push_back(myo::Pose::fingersSpread);
    gestureToMouseSequence.push_back(myo::Pose::thumbToPinky);
    
    mouseToKeyboardSequence.push_back(myo::Pose::waveOut);
    mouseToKeyboardSequence.push_back(myo::Pose::thumbToPinky);
    
    keyboardToMouseSequence.push_back(myo::Pose::waveOut);
    keyboardToMouseSequence.push_back(myo::Pose::thumbToPinky);

    sequenceCount = 0;
    stateProgressMaxDeltaTime = DEFAULT_PROG_MAX_DELTA;
    activeSeq = activeSequence::NONE;
}

GestureFilter::StateHandler::~StateHandler()
{
}

bool GestureFilter::StateHandler::updateState(myo::Pose::Type gesture)
{
    if (gesture == myo::Pose::Type::rest)
    {
        return false;
    }

    midasMode currentState = parent.controlStateHandle->getMode();
    
    bool willTransition = false;
    midasMode nextMode = midasMode::LOCK_MODE;
    clock_t now = clock();

    if (activeSeq != activeSequence::NONE)
    {
        if (now - stateProgressBaseTime > stateProgressMaxDeltaTime)
        {
            std::cout << "timed out of previous sequence attempt." << std::endl;
            // reset and pass through to rest of updateState check.
            activeSeq = activeSequence::NONE;
            sequenceCount = 0;
        }
    }

    switch (currentState)
    {
    case(LOCK_MODE) :
        // Can only unlock from this locked state
        if (activeSeq == activeSequence::UNLOCK)
        {
            if (gesture == unlockSequence.at(sequenceCount))
            {
                //std::cout << "In lockMode, progressed to unlock - sequenceCount: " << sequenceCount << std::endl;
                sequenceCount++;
                stateProgressBaseTime = now;
            }
        }
        else
        {
            // Attempting to start a legal sequence.
            // Only one possible sequence to get out of locked state.
            if (gesture == unlockSequence.at(0))
            {
                // Activated a sequence
                //std::cout << "In lockMode, started progressed to unlock - sequenceCount: " << sequenceCount << std::endl;
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
            std::cout << "Entering Mouse Mode." << std::endl;
            nextMode = MOUSE_MODE;
            willTransition = true;
        }

        break;
    case(MOUSE_MODE) :
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
            if (gesture == lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::LOCK;
            }
            else if (gesture == mouseToGestureSequence.at(0)) // TODO - FIX THIS -- CURRENTLY THIS WILL NOT ALLOW 2 PATTERNS WITH IDENTICAL FIRST SIGNALS TO FUNCTION.
            {
                // Activated a sequence
                activeSeq = activeSequence::MOUSE_TO_GEST;
            }
            else if (gesture == mouseToKeyboardSequence.at(0))
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
            std::cout << "Entering Lock Mode." << std::endl;
            nextMode = LOCK_MODE;
            willTransition = true;
        }
        else if (activeSeq == activeSequence::MOUSE_TO_GEST && sequenceCount == mouseToGestureSequence.size())
        {
            // Succeeded in completing mouse to gesture sequence
            std::cout << "Entering Gesture Mode." << std::endl;
            nextMode = GESTURE_MODE;
            willTransition = true;
        }
        else if (activeSeq == activeSequence::MOUSE_TO_KYBRD && sequenceCount == mouseToKeyboardSequence.size())
        {
            // Succeeded in completing mouse to keyboard sequence
            std::cout << "Entering Keyboard Mode." << std::endl;
            nextMode = KEYBOARD_MODE;
            willTransition = true;
        }

        break;
    case(GESTURE_MODE) :
        if (activeSeq == activeSequence::LOCK && gesture == lockSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else if (activeSeq == activeSequence::GEST_TO_MOUSE && gesture == gestureToMouseSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else
        {
            // Attempting to start a legal sequence.
            sequenceCount++;
            stateProgressBaseTime = now;
            if (gesture == lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::LOCK;
            }
            else if (gesture == gestureToMouseSequence.at(0)) // TODO - FIX THIS -- CURRENTLY THIS WILL NOT ALLOW 2 PATTERNS WITH IDENTICAL FIRST SIGNALS TO FUNCTION.
            {
                // Activated a sequence
                activeSeq = activeSequence::GEST_TO_MOUSE;
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
            std::cout << "Entering Lock Mode." << std::endl;
            nextMode = LOCK_MODE;
            willTransition = true;
        }
        else if (activeSeq == activeSequence::GEST_TO_MOUSE && sequenceCount == gestureToMouseSequence.size())
        {
            // Succeeded in completing mouse to gesture sequence
            std::cout << "Entering Mouse Mode." << std::endl;
            nextMode = MOUSE_MODE;
            willTransition = true;
        }

        break;
    case(KEYBOARD_MODE) :
        if (activeSeq == activeSequence::LOCK && gesture == lockSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else if (activeSeq == activeSequence::KYBRD_TO_MOUSE && gesture == keyboardToMouseSequence.at(sequenceCount))
        {
            sequenceCount++;
            stateProgressBaseTime = now;
        }
        else
        {
            // Attempting to start a legal sequence.
            sequenceCount++;
            stateProgressBaseTime = now;
            if (gesture == lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSequence::LOCK;
            }
            else if (gesture == keyboardToMouseSequence.at(0)) // TODO - FIX THIS -- CURRENTLY THIS WILL NOT ALLOW 2 PATTERNS WITH IDENTICAL FIRST SIGNALS TO FUNCTION.
            {
                // Activated a sequence
                activeSeq = activeSequence::KYBRD_TO_MOUSE;
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
            std::cout << "Entering Lock Mode." << std::endl;
            nextMode = LOCK_MODE;
            willTransition = true;
        }
        else if (activeSeq == activeSequence::KYBRD_TO_MOUSE && sequenceCount == keyboardToMouseSequence.size())
        {
            // Succeeded in completing mouse to gesture sequence
            std::cout << "Entering Mouse Mode." << std::endl;
            nextMode = MOUSE_MODE;
            willTransition = true;
        }

        break;
    }

    // Reset if succeeded in transition
    if (willTransition)
    {
        std::cout << "Transitioning to " << nextMode << std::endl;
        parent.controlStateHandle->setMode(nextMode);
        activeSeq = activeSequence::NONE;
        sequenceCount = 0;
        return true;
    }

    return false;
}

void GestureFilter::StateHandler::setStateProgressMaxDeltaTime(clock_t newTime)
{
    stateProgressBaseTime = newTime;
}

clock_t GestureFilter::StateHandler::getStateProgressMaxDeltaTime(void)
{
    return stateProgressBaseTime;
}