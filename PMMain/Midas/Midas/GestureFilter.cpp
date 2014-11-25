#include "GestureFilter.h"
#include <time.h>

// TODO: Refactor cases to modularize into seperate handler functions!

GestureFilter::GestureFilter(ControlState* controlState, clock_t timeDel) : timeDelta(timeDel), lastPoseType(Pose::rest),
    lastTime(0), controlStateHandle(controlState), stateHandler(*this)
{
}

GestureFilter::~GestureFilter()
{
}

void GestureFilter::process()
{
    clock_t timeFromLastPost = 0;
    filterDataMap input = Filter::getInput();
    Pose::Type gesture = boost::any_cast<Pose::Type>(input[GESTURE_INPUT]);
    
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);

    if (gesture != lastPoseType)
    {
        // The user's gesture has changed.
        if (gesture == Pose::rest && controlStateHandle->getMode() != midasMode::LOCK_MODE)
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
        timeFromLastPost = clock() - lastTime;
        lastTime = clock();
    }

    if (timeFromLastPost >= timeDelta)
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
            if (controlStateHandle->getMode() != midasMode::LOCK_MODE)
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

commandData GestureFilter::translateGesture(Pose::Type pose)
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


//NOTE - IF THESE SEQUENCES ARE CHANGED, GestureFilter_Test must be changed as well!
GestureFilter::StateHandler::StateHandler(GestureFilter& parent) : parent(parent)
{
    unlockSequence.push_back(Pose::thumbToPinky);
    unlockSequence.push_back(Pose::waveIn);
    unlockSequence.push_back(Pose::waveOut);
    
    lockSequence.push_back(Pose::waveIn);
    lockSequence.push_back(Pose::thumbToPinky);

    // None of the following modes actually have functionality, so their 
    // state transition sequences are arbitrary and incomplete. TODO.
    mouseToGestureSequence.push_back(Pose::thumbToPinky);
    
    gestureToMouseSequence.push_back(Pose::thumbToPinky);
    
    mouseToKeyboardSequence.push_back(Pose::waveOut);
    mouseToKeyboardSequence.push_back(Pose::waveIn);
    
    keyboardToMouseSequence.push_back(Pose::waveOut);
    keyboardToMouseSequence.push_back(Pose::waveIn);

    sequenceCount = 0;
    stateProgressMaxDeltaTime = DEFAULT_PROG_MAX_DELTA;
    activeSeq = activeSequence::NONE;
}

GestureFilter::StateHandler::~StateHandler()
{
}

bool GestureFilter::StateHandler::updateState(Pose::Type gesture)
{
    if (gesture == Pose::Type::rest)
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
        checkProgressInSequence(activeSequence::UNLOCK, unlockSequence, gesture, progressSeq, now);
        
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
    case(MOUSE_MODE) :
        checkProgressInSequence(activeSequence::LOCK, lockSequence, gesture, progressSeq, now);
        checkProgressInSequence(activeSequence::MOUSE_TO_GEST, mouseToGestureSequence, gesture, progressSeq, now);
        checkProgressInSequence(activeSequence::MOUSE_TO_KYBRD, mouseToKeyboardSequence, gesture, progressSeq, now);
        
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
        checkProgressInSequence(activeSequence::LOCK, lockSequence, gesture, progressSeq, now);
        checkProgressInSequence(activeSequence::GEST_TO_MOUSE, gestureToMouseSequence, gesture, progressSeq, now);
        
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
        checkProgressInSequence(activeSequence::LOCK, lockSequence, gesture, progressSeq, now);
        checkProgressInSequence(activeSequence::KYBRD_TO_MOUSE, keyboardToMouseSequence, gesture, progressSeq, now);
        
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
    stateProgressMaxDeltaTime = newTime;
}

clock_t GestureFilter::StateHandler::getStateProgressMaxDeltaTime(void)
{
    return stateProgressMaxDeltaTime;
}

bool GestureFilter::StateHandler::satisfyStateChange(activeSequence desiredSeq, std::vector<Pose::Type> sequence,
    Pose::Type gesture, midasMode desiredMode, midasMode& nextMode, bool& willTransition)
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

void GestureFilter::StateHandler::checkProgressInSequence(activeSequence desiredSeq, std::vector<Pose::Type> sequence,
    Pose::Type gesture, bool& progressSeq, clock_t now)
{
    if (((activeSeq & desiredSeq) != activeSequence::NONE) &&
        sequenceCount < sequence.size() && 
        gesture == sequence.at(sequenceCount))
    {
        progressSeq = true;
        stateProgressBaseTime = now;
    }
    else
    {
        // Take desired sequence flag out of mask, since it's gesture was missed in the sequence.
        activeSeq = activeSeq & ~desiredSeq;
    }
}

