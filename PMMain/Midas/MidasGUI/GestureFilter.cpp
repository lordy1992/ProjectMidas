#include "GestureFilter.h"
#include <time.h>
#include <thread>
#include "MyoCommon.h"

ControlState* GestureFilter::controlStateHandle;

GestureFilter::GestureFilter(ControlState* controlState, clock_t timeDel) : timeDelta(timeDel), lastPoseType(Pose::rest),
    lastTime(0), gestSeqRecorder(midasMode::LOCK_MODE, SEQ_TIMEOUT_LENGTH)
{
    registerMouseSequences();
    registerKeyboardSequences();
    registerStateSequences();

    controlStateHandle = controlState;

    setupCallbackThread(this);
}

GestureFilter::~GestureFilter()
{
}

void GestureFilter::process()
{
    clock_t timeFromLastPose = 0;
    filterDataMap input = Filter::getInput();
    Pose::Type gesture = boost::any_cast<Pose::Type>(input[GESTURE_INPUT]);
    
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);
    Filter::clearOutput();

    // First, filter based on "hold time" in a specific gesture.
    timeFromLastPose = clock() - lastTime;
    if (timeFromLastPose < timeDelta)
    {
        // early exit due to too frequent fluctuation
        return;
    }

    commandData response;
    SequenceStatus ss;
    ss = gestSeqRecorder.progressSequence(gesture, *controlStateHandle, response);
    if (gesture != Pose::Type::rest)
    {
        // Handle state info first, as it is most important.
        if (response.type == commandType::NONE)
        {
            // terminate filter pipeline, as nothing to add
            Filter::setFilterStatus(filterStatus::END_CHAIN);
        }
        lastResponseType = response.type; // only care about non-rest responses. // JORDEN TODO - see if theres a better way to do this so that the "tapped" mouse buttons will behave better (currently only "immediate" works nicely with this code)
        // maybe just need to move this out of the if statment since responses are always returned now..? TODO test this theory after the meeting Jan 20.
        // Nah... Really need to do what i was already planning, which is to extend the mouseCtrl class to include a "press and release" command
        // like the kybrdCtrl class which includes a short delay, but completes the desired action entirely.
    }
    else
    {
        if (response.type == commandType::NONE)
        {
            // No state stuff to do, so handle other special cases!
            if (lastResponseType == commandType::MOUSE_CMD)
            {
                handleMouseRelease();
            }
            else
            {
                // terminate filter pipeline, as nothing to add
                Filter::setFilterStatus(filterStatus::END_CHAIN);
            }
        }
    }

    if (response.type == commandType::STATE_CHANGE)
    {
        handleStateChange(response);
    }
    else if (response.type == commandType::MOUSE_CMD)
    {
        handleMouseCommand(response);
    }
    else if (response.type == commandType::KYBRD_CMD || response.type == commandType::KYBRD_GUI_CMD)
    {
        handleKybrdCommand(response);
    }

    lastPoseType = gesture;
    lastTime = clock();
}

void GestureFilter::registerMouseSequences(void)
{
    // Register sequence to left click in mouse mode and gesture mode
    sequence clickSeq;
    clickSeq.push_back(SeqElement(MYO_GESTURE_LEFT_MOUSE, SeqElement::PoseLength::IMMEDIATE));
    commandData clickResp;
    clickResp.name = "Left Click";
    clickResp.type = commandType::MOUSE_CMD;
    clickResp.action.mouse = mouseCmds::LEFT_CLICK;
    int ss = (int)gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp);
    //clickSeq.at(0).poseLen = SeqElement::PoseLength::TAP; // Jorden. TODO. Not quite this, or it can 'stick on click'
    //ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp);

    // Register sequence to right click in mouse mode and gesture mode
    clickSeq.clear();
    clickSeq.push_back(SeqElement(MYO_GESTURE_RIGHT_MOUSE, SeqElement::PoseLength::IMMEDIATE));
    clickResp.name = "Right Click";
    clickResp.type = commandType::MOUSE_CMD;
    clickResp.action.mouse = mouseCmds::RIGHT_CLICK;
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp);
    //clickSeq.at(0).poseLen = SeqElement::PoseLength::TAP; // Jorden. TODO. Not quite this, or it can 'stick on click'
    //ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp);

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException: mouse");
    }
}

void GestureFilter::registerKeyboardSequences(void)
{
    sequence kybrdGUISequence;
    commandData kybrdGUIResponse;

    // register arm specific commands first
    kybrdGUIResponse.name = "Swap Ring Focus";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::SWAP_RING_FOCUS;
    //if (left arm) // Todo, figure out an elegent way to access arm data here.
    //{
    //    kybrdGUISequence.push_back(SeqElement(Pose::Type::waveIn));
    //}
    //else
    {
        kybrdGUISequence.push_back(SeqElement(Pose::Type::waveOut));
    }
    int ss = (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse);
    kybrdGUIResponse.name = "Backspace";
    kybrdGUIResponse.type = commandType::KYBRD_CMD;
    kybrdGUIResponse.action.kybd = kybdCmds::BACKSPACE;
    //if (left arm) // Todo, figure out an elegent way to access arm data here.
    //{
    //    kybrdGUISequence[0] = (SeqElement(Pose::Type::waveOut));
    //}
    //else
    {
        kybrdGUISequence[0] = (SeqElement(Pose::Type::waveIn));
    }
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse);

    // register the rest of the keyboard commands
    kybrdGUIResponse.name = "Select";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::SELECT;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist));
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse);

    kybrdGUIResponse.name = "Hold Select";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::HOLD_SELECT;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist, SeqElement::PoseLength::HOLD));
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse);

    kybrdGUIResponse.name = "Change Wheels";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::CHANGE_WHEELS;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fingersSpread));
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse);

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException: keyboard");
    }
}

void GestureFilter::registerStateSequences(void)
{
    // Register sequence from lock to Mouse Mode
    sequence lockToMouseSeq;
    lockToMouseSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    lockToMouseSeq.push_back(SeqElement(Pose::Type::waveIn));
    lockToMouseSeq.push_back(SeqElement(Pose::Type::waveOut));
    commandData lockToMouseResponse;
    lockToMouseResponse.name = "Unlock";
    lockToMouseResponse.type = commandType::STATE_CHANGE;
    lockToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    int ss = (int)gestSeqRecorder.registerSequence(midasMode::LOCK_MODE, lockToMouseSeq, lockToMouseResponse);

    // Register sequence from Mouse Mode to Gesture Mode
    sequence mouseToGestSeq;
    mouseToGestSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    commandData mouseToGestResponse;
    mouseToGestResponse.name = "Mouse To Gesture";
    mouseToGestResponse.type = commandType::STATE_CHANGE;
    mouseToGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, mouseToGestSeq, mouseToGestResponse);

    // Register sequence from Mouse Mode to Keyboard Mode
    sequence mouseToKybrdSeq;
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveOut));
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveIn));
    commandData mouseToKybrdResponse;
    mouseToKybrdResponse.name = "Mouse To Keyboard";
    mouseToKybrdResponse.type = commandType::STATE_CHANGE;
    mouseToKybrdResponse.action.mode = midasMode::KEYBOARD_MODE;

    ss |= (int)gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, mouseToKybrdSeq, mouseToKybrdResponse);

    // Register sequence from Gesture Mode to Mouse Mode
    sequence gestureToMouseSeq;
    gestureToMouseSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    commandData gestureToMouseResponse;
    gestureToMouseResponse.name = "Gesture To Mouse";
    gestureToMouseResponse.type = commandType::STATE_CHANGE;
    gestureToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, gestureToMouseSeq, gestureToMouseResponse);

    // Register sequence from Keyboard Mode to Mouse Mode
    sequence kybrdToMouseSeq;
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::waveIn));
    commandData kybrdToMouseResponse;
    kybrdToMouseResponse.name = "Keyboard To Mouse";
    kybrdToMouseResponse.type = commandType::STATE_CHANGE;
    kybrdToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    ss |= (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdToMouseSeq, kybrdToMouseResponse);

    // Register sequence from most Modes to Lock Mode.
    sequence toLockSeq;
    toLockSeq.push_back(SeqElement(Pose::Type::waveIn));
    toLockSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    commandData toLockResponse;
    toLockResponse.type = commandType::STATE_CHANGE;
    toLockResponse.action.mode = midasMode::LOCK_MODE;
    // From Mouse:
    toLockResponse.name = "Mouse To Lock";
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, toLockSeq, toLockResponse);
    // From Gesture:
    toLockResponse.name = "Gesture To Lock";
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toLockSeq, toLockResponse);
    // From Keyboard:
    toLockResponse.name = "Keyboard To Lock"; // OVERRIDE for Keyboard.
    toLockSeq[0] = (SeqElement(Pose::Type::thumbToPinky));
    toLockSeq[1] = (SeqElement(Pose::Type::waveOut));
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, toLockSeq, toLockResponse);

    // Register sequence from Gesture Mode to Gesture Hold Modes
    sequence toHoldGestSeq;
    toHoldGestSeq.push_back(SeqElement(Pose::Type::thumbToPinky, SeqElement::PoseLength::HOLD));
    commandData toHoldGestResponse;
    toHoldGestResponse.name = "Gesture to Hold Gesture X";
    toHoldGestResponse.type = commandType::STATE_CHANGE;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_ONE;

    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::fingersSpread;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_TWO;
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::fist;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_THREE;
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::waveIn;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FOUR;
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::waveOut;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FIVE;
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    // Register sequences back to Gesture Mode from Gesture Hold Modes

    sequence fromHoldGestSeq;
    fromHoldGestSeq.push_back(SeqElement(Pose::Type::rest, SeqElement::PoseLength::IMMEDIATE));
    commandData fromHoldGestResponse;
    fromHoldGestResponse.name = "Gesture from Hold Gesture X";
    fromHoldGestResponse.type = commandType::STATE_CHANGE;
    fromHoldGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_ONE, fromHoldGestSeq, fromHoldGestResponse);
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_TWO, fromHoldGestSeq, fromHoldGestResponse);
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_THREE, fromHoldGestSeq, fromHoldGestResponse);
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_FOUR, fromHoldGestSeq, fromHoldGestResponse);
    ss |= (int)gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_FIVE, fromHoldGestSeq, fromHoldGestResponse);

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }
}

void GestureFilter::handleStateChange(commandData response)
{
    if (response.type != commandType::STATE_CHANGE)
    {
        // Should NOT have made it here
        return;
    }

    std::cout << "Transitioning to state: " << response.action.mode << std::endl;
    controlStateHandle->setMode(response.action.mode);
    
    return;
}

void GestureFilter::handleMouseCommand(commandData response)
{
    if (controlStateHandle->getMode() == midasMode::MOUSE_MODE ||
        controlStateHandle->getMode() == midasMode::GESTURE_MODE)
    {
        filterDataMap outputToSharedCommandData;
        commandData command;
        command = response;

        outputToSharedCommandData[COMMAND_INPUT] = command;
        Filter::setOutput(outputToSharedCommandData);
    }
}

void GestureFilter::handleKybrdCommand(commandData response)
{
    if (controlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        filterDataMap outputToSharedCommandData;
        commandData command;
        command = response;

        outputToSharedCommandData[COMMAND_INPUT] = command;
        Filter::setOutput(outputToSharedCommandData);
    }
}

void GestureFilter::handleMouseRelease()
{
    if (controlStateHandle->getMode() == midasMode::MOUSE_MODE ||
        controlStateHandle->getMode() == midasMode::GESTURE_MODE)
    {
        filterDataMap outputToSharedCommandData;
        commandData command;
        command.type = MOUSE_CMD;

        if (lastPoseType == MYO_GESTURE_LEFT_MOUSE)
        {
            command.action.mouse = LEFT_RELEASE;
            outputToSharedCommandData[COMMAND_INPUT] = command;
            Filter::setOutput(outputToSharedCommandData);
            return;
        }
        else if (lastPoseType == MYO_GESTURE_RIGHT_MOUSE)
        {
            command.action.mouse = RIGHT_RELEASE;
            outputToSharedCommandData[COMMAND_INPUT] = command;
            Filter::setOutput(outputToSharedCommandData);
            return;
        }
    }
}

void setupCallbackThread(GestureFilter *gf)
{
    std::thread callbackThread(callbackThreadWrapper, gf);
    callbackThread.detach();
}

void callbackThreadWrapper(GestureFilter *gf)
{
    std::chrono::milliseconds period(SLEEP_LEN);
    do {
        std::this_thread::sleep_for(period);
        gf->getGestureSeqRecorder()->checkProgressBaseTime();

        commandData response;
        gf->getGestureSeqRecorder()->progressSequenceTime(SLEEP_LEN, response);
        if (response.type == commandType::STATE_CHANGE)
        {
            GestureFilter::handleStateChange(response);
        }
        //else if (response.type == commandType::MOUSE_CMD)
        //{
        //    handleMouseCommand(response);
        //}
        //else if (response.type == commandType::KYBRD_CMD)
        //{
        //    handleKybrdCommand(response);
        //}
        // TODO - figure out if this is necessary here... cant make work easily as functions
        // cant simply be converted to static. But... probably should make it work... Need to figure out.

    } while (true);
}
