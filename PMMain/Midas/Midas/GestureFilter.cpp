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
    
    mouseToKeyboardSequence.push_back(myo::Pose::fingersSpread);
    mouseToKeyboardSequence.push_back(myo::Pose::fist);
    mouseToKeyboardSequence.push_back(myo::Pose::thumbToPinky);
    //mouseToKeyboardSequence.push_back(myo::Pose::waveOut);
    //mouseToKeyboardSequence.push_back(myo::Pose::thumbToPinky);
    
    keyboardToMouseSequence.push_back(myo::Pose::fingersSpread);
    keyboardToMouseSequence.push_back(myo::Pose::fist);
    keyboardToMouseSequence.push_back(myo::Pose::thumbToPinky);
    //keyboardToMouseSequence.push_back(myo::Pose::waveOut);
    //keyboardToMouseSequence.push_back(myo::Pose::thumbToPinky);

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
    bool progressSeq = false;
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
        if (((activeSeq & activeSequence::UNLOCK) != activeSequence::NONE) && sequenceCount < unlockSequence.size() && gesture == unlockSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::UNLOCK;
        }
        if (activeSeq == activeSequence::NONE)
        {
            // Attempting to start a legal sequence.
            progressSeq = true;
            stateProgressBaseTime = now;
            if (gesture == unlockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::UNLOCK;
            }
            
            if (activeSeq == activeSequence::NONE)
            {
                // Failed to activate a sequence
                progressSeq = false;
                sequenceCount = 0;
            }
        }

        if (progressSeq)
        {
            sequenceCount++;
        }

        if (satisfyStateChange(activeSequence::UNLOCK, unlockSequence, gesture, MOUSE_MODE, nextMode, willTransition))
        {
            // Succeeded in completing lock sequence
            std::cout << "Entering Mouse Mode." << std::endl;
        }

        break;
        ///////////////////////////////////////////////////////////////////////////////////////
        // Can only unlock from this locked state
       //if (((activeSeq & activeSequence::UNLOCK) != activeSequence::NONE))
       //{
       //    if (gesture == unlockSequence.at(sequenceCount))
       //    {
       //        progressSeq = true;
       //        stateProgressBaseTime = now;
       //    }
       //    else
       //    {
       //        activeSeq = activeSeq & ~activeSequence::UNLOCK;
       //    }
       //}
       //if (activeSeq == activeSequence::NONE)
       //{
       //    // Attempting to start a legal sequence.
       //    // Only one possible sequence to get out of locked state.
       //    progressSeq = true;
       //    if (gesture == unlockSequence.at(0))
       //    {
       //        // Activated a sequence
       //        activeSeq = activeSeq | activeSequence::UNLOCK;
       //        stateProgressBaseTime = now;
       //    }
       //    
       //    if (activeSeq == activeSequence::NONE)
       //    {
       //        // Failed to activate a sequence
       //        progressSeq = false;
       //        sequenceCount = 0;
       //    }
       //}
       //
       //if (progressSeq)
       //{
       //    sequenceCount++;
       //}
       //
       //if (sequenceCount == unlockSequence.size()) // TODO - conform this statement to all other blocks!
       //{
       //    // Succeeded in completing unlock sequence
       //    std::cout << "Entering Mouse Mode." << std::endl;
       //    nextMode = MOUSE_MODE;
       //    willTransition = true;
       //}
       //
       //break;
    case(MOUSE_MODE) :
        if (((activeSeq & activeSequence::LOCK) != activeSequence::NONE) && sequenceCount < lockSequence.size() && gesture == lockSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::LOCK;
        }
        if (((activeSeq & activeSequence::MOUSE_TO_GEST) != activeSequence::NONE) && sequenceCount < mouseToGestureSequence.size() && gesture == mouseToGestureSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::MOUSE_TO_GEST;
        }
        if (((activeSeq & activeSequence::MOUSE_TO_KYBRD) != activeSequence::NONE) && sequenceCount < mouseToKeyboardSequence.size() && gesture == mouseToKeyboardSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::MOUSE_TO_KYBRD;
        }
        
        if (activeSeq == activeSequence::NONE)
        {
            // Attempting to start a legal sequence.
            progressSeq = true;
            stateProgressBaseTime = now;
            if (gesture == lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::LOCK;
            }
            if (gesture == mouseToGestureSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::MOUSE_TO_GEST;
            }
            if (gesture == mouseToKeyboardSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::MOUSE_TO_KYBRD;
            }
            
            if (activeSeq == activeSequence::NONE)
            {
                // Failed to activate a sequence
                progressSeq = false;
                sequenceCount = 0;
            }
        }

        if (progressSeq)
        {
            sequenceCount++;
        }

        if (satisfyStateChange(activeSequence::LOCK, lockSequence, gesture, LOCK_MODE, nextMode, willTransition))
        {
            // Succeeded in completing lock sequence
            std::cout << "Entering Lock Mode." << std::endl;
        }
        if (satisfyStateChange(activeSequence::MOUSE_TO_GEST, mouseToGestureSequence, gesture, GESTURE_MODE, nextMode, willTransition))
        {
            // Succeeded in completing mouse to gesture sequence
            std::cout << "Entering Gesture Mode." << std::endl;
        }
        if (satisfyStateChange(activeSequence::MOUSE_TO_KYBRD, mouseToKeyboardSequence, gesture, KEYBOARD_MODE, nextMode, willTransition))
        {
            // Succeeded in completing mouse to keyboard sequence
            std::cout << "Entering Keyboard Mode." << std::endl;
        }

        break;
    case(GESTURE_MODE) :
        if (((activeSeq & activeSequence::LOCK) != activeSequence::NONE) && sequenceCount < lockSequence.size() && gesture == lockSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::LOCK;
        }
        if (((activeSeq & activeSequence::GEST_TO_MOUSE) != activeSequence::NONE) && sequenceCount < gestureToMouseSequence.size() && gesture == gestureToMouseSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::GEST_TO_MOUSE;
        }
        if (activeSeq == activeSequence::NONE)
        {
            // Attempting to start a legal sequence.
            progressSeq = true;
            stateProgressBaseTime = now;
            if (gesture == lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::LOCK;
            }
            if (gesture == gestureToMouseSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::GEST_TO_MOUSE;
            }
            
            if (activeSeq == activeSequence::NONE)
            {
                // Failed to activate a sequence
                progressSeq = false;
                sequenceCount = 0;
            }
        }

        if (progressSeq)
        {
            sequenceCount++;
        }

        if (satisfyStateChange(activeSequence::LOCK, lockSequence, gesture, LOCK_MODE, nextMode, willTransition))
        {
            // Succeeded in completing lock sequence
            std::cout << "Entering Lock Mode." << std::endl;
        }
        if (satisfyStateChange(activeSequence::GEST_TO_MOUSE, gestureToMouseSequence, gesture, MOUSE_MODE, nextMode, willTransition))
        {
            // Succeeded in completing gesture to mouse sequence
            std::cout << "Entering Mouse Mode." << std::endl;
        }

        break;
    case(KEYBOARD_MODE) :
        if (((activeSeq & activeSequence::LOCK) != activeSequence::NONE) && sequenceCount < lockSequence.size() && gesture == lockSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::LOCK;
        }
        if (((activeSeq & activeSequence::KYBRD_TO_MOUSE) != activeSequence::NONE) && sequenceCount < keyboardToMouseSequence.size() && gesture == keyboardToMouseSequence.at(sequenceCount))
        {
            progressSeq = true;
            stateProgressBaseTime = now;
        }
        else
        {
            activeSeq = activeSeq & ~activeSequence::KYBRD_TO_MOUSE;
        }
        if (activeSeq == activeSequence::NONE)
        {
            // Attempting to start a legal sequence.
            progressSeq = true;
            stateProgressBaseTime = now;
            if (gesture == lockSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::LOCK;
            }
            if (gesture == keyboardToMouseSequence.at(0))
            {
                // Activated a sequence
                activeSeq = activeSeq | activeSequence::KYBRD_TO_MOUSE;
            }
            
            if (activeSeq == activeSequence::NONE)
            {
                // Failed to activate a sequence
                progressSeq = false;
                sequenceCount = 0;
            }
        }

        if (progressSeq)
        {
            sequenceCount++;
        }

        if (satisfyStateChange(activeSequence::LOCK, lockSequence, gesture, LOCK_MODE, nextMode, willTransition))
        {
            // Succeeded in completing lock sequence
            std::cout << "Entering Lock Mode." << std::endl;
        }
        if (satisfyStateChange(activeSequence::KYBRD_TO_MOUSE, keyboardToMouseSequence, gesture, MOUSE_MODE, nextMode, willTransition))
        {
            // Succeeded in completing keyboard to mosue sequence
            std::cout << "Entering Mouse Mode." << std::endl;
        }

        break;
    }

    // Reset if succeeded in transition
    if (willTransition)
    {
        std::cout << "Transitioning to " << nextMode << std::endl;
        parent.controlStateHandle->setMode(nextMode);
        activeSeq = activeSequence::NONE;
        progressSeq = false;
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

bool GestureFilter::StateHandler::satisfyStateChange(activeSequence desiredSeq, std::vector<myo::Pose::Type> sequence,
    myo::Pose::Type gesture, midasMode desiredMode, midasMode& nextMode, bool& willTransition)
{
    // Ensure current sequence is following the desired sequence to change states to, and that
    // all elements have been matched, and specifically, the last element is a match
    // -- This last check is to enable patterns to only differ by their last pose.
    if (((activeSeq & desiredSeq) != activeSequence::NONE) &&
        (sequenceCount == sequence.size()) &&
        (gesture == sequence.at(sequenceCount - 1))
        )
    {
        nextMode = desiredMode;
        willTransition = true;
        return true;
    }

    return false;
}