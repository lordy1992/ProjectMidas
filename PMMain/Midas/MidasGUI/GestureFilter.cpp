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
    // TODO - Setup constant callback function that sends signals to the gestSeqRecorder to indicate time, so that things like
    // Hold sequences can be recognized, and sequence timeouts will be up to date.
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

    sequenceResponse response;
    SequenceStatus ss;
    ss = gestSeqRecorder.progressSequence(gesture, *controlStateHandle, response);
    if (gesture != Pose::Type::rest)
    {
        // Handle state info first, as it is most important.
        if (response.responseType == ResponseType::NONE)
        {
            // terminate filter pipeline, as nothing to add
            Filter::setFilterStatus(filterStatus::END_CHAIN);
        }
        lastResponseType = response.responseType; // only care about non-rest responses.
    }
    else
    {
        if (response.responseType == ResponseType::NONE)
        {
            // No state stuff to do, so handle other special cases!
            if (lastResponseType == ResponseType::MOUSE_CMD)
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

    if (response.responseType == ResponseType::STATE_CHANGE)
    {
        handleStateChange(response);
    }
    else if (response.responseType == ResponseType::MOUSE_CMD)
    {
        handleMouseCommand(response);
    }
    else if (response.responseType == ResponseType::KYBRD_CMD)
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
    clickSeq.push_back(seqElement(MYO_GESTURE_LEFT_MOUSE));
    sequenceResponse clickResp;
    clickResp.responseName = "Left Click";
    clickResp.responseType = ResponseType::MOUSE_CMD;
    clickResp.responseAction.mouse = mouseCmds::LEFT_CLICK;
    SequenceStatus ss = gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp);
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp);

    // Register sequence to right click in mouse mode and gesture mode
    clickSeq.clear();
    clickSeq.push_back(seqElement(MYO_GESTURE_RIGHT_MOUSE));
    clickResp.responseName = "Right Click";
    clickResp.responseType = ResponseType::MOUSE_CMD;
    clickResp.responseAction.mouse = mouseCmds::RIGHT_CLICK;
    ss = gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp);
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp);

    if (ss != SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }
}

void GestureFilter::registerKeyboardSequences(void)
{
    // Nothing to register right now. Always could though :)
}

void GestureFilter::registerStateSequences(void)
{
    // Register sequence from lock to Mouse Mode
    sequence lockToMouseSeq;
    lockToMouseSeq.push_back(seqElement(Pose::Type::thumbToPinky));
    lockToMouseSeq.push_back(seqElement(Pose::Type::waveIn));
    lockToMouseSeq.push_back(seqElement(Pose::Type::waveOut));
    sequenceResponse lockToMouseResponse;
    lockToMouseResponse.responseName = "Unlock";
    lockToMouseResponse.responseType = ResponseType::STATE_CHANGE;
    lockToMouseResponse.responseAction.mode = midasMode::MOUSE_MODE;

    SequenceStatus ss = gestSeqRecorder.registerSequence(midasMode::LOCK_MODE, lockToMouseSeq, lockToMouseResponse);

    // Register sequence from Mouse Mode to Gesture Mode
    sequence mouseToGestSeq;
    mouseToGestSeq.push_back(seqElement(Pose::Type::thumbToPinky));
    sequenceResponse mouseToGestResponse;
    mouseToGestResponse.responseName = "Mouse To Gesture";
    mouseToGestResponse.responseType = ResponseType::STATE_CHANGE;
    mouseToGestResponse.responseAction.mode = midasMode::GESTURE_MODE;

    ss = gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, mouseToGestSeq, mouseToGestResponse);

    // Register sequence from Mouse Mode to Keyboard Mode
    sequence mouseToKybrdSeq;
    mouseToKybrdSeq.push_back(seqElement(Pose::Type::waveOut));
    mouseToKybrdSeq.push_back(seqElement(Pose::Type::waveIn));
    sequenceResponse mouseToKybrdResponse;
    mouseToKybrdResponse.responseName = "Mouse To Keyboard";
    mouseToKybrdResponse.responseType = ResponseType::STATE_CHANGE;
    mouseToKybrdResponse.responseAction.mode = midasMode::KEYBOARD_MODE;

    ss = gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, mouseToKybrdSeq, mouseToKybrdResponse);

    // Register sequence from Gesture Mode to Mouse Mode
    sequence gestureToMouseSeq;
    gestureToMouseSeq.push_back(seqElement(Pose::Type::thumbToPinky));
    sequenceResponse gestureToMouseResponse;
    gestureToMouseResponse.responseName = "Gesture To Mouse";
    gestureToMouseResponse.responseType = ResponseType::STATE_CHANGE;
    gestureToMouseResponse.responseAction.mode = midasMode::MOUSE_MODE;

    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, gestureToMouseSeq, gestureToMouseResponse);

    // Register sequence from Keyboard Mode to Mouse Mode
    sequence kybrdToMouseSeq;
    kybrdToMouseSeq.push_back(seqElement(Pose::Type::waveOut));
    kybrdToMouseSeq.push_back(seqElement(Pose::Type::waveIn));
    sequenceResponse kybrdToMouseResponse;
    kybrdToMouseResponse.responseName = "Keyboard To Mouse";
    kybrdToMouseResponse.responseType = ResponseType::STATE_CHANGE;
    kybrdToMouseResponse.responseAction.mode = midasMode::MOUSE_MODE;

    ss = gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, kybrdToMouseSeq, kybrdToMouseResponse);

    // Register sequence from ALL Modes to Lock Mode.
    sequence toLockSeq;
    toLockSeq.push_back(seqElement(Pose::Type::waveIn));
    toLockSeq.push_back(seqElement(Pose::Type::thumbToPinky));
    sequenceResponse toLockResponse;
    toLockResponse.responseType = ResponseType::STATE_CHANGE;
    toLockResponse.responseAction.mode = midasMode::LOCK_MODE;
    // From Mouse:
    toLockResponse.responseName = "Mouse To Lock";
    ss = gestSeqRecorder.registerSequence(midasMode::MOUSE_MODE, toLockSeq, toLockResponse);
    // From Gesture:
    toLockResponse.responseName = "Gesture To Lock";
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toLockSeq, toLockResponse);
    // From Keyboard:
    toLockResponse.responseName = "Keyboard To Lock";
    ss = gestSeqRecorder.registerSequence(midasMode::KEYBOARD_MODE, toLockSeq, toLockResponse);

    if (ss != SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }

    // Register sequence from Gesture Mode to Gesture Hold Modes
    sequence toHoldGestSeq;
    toHoldGestSeq.push_back(seqElement(Pose::Type::thumbToPinky, true));
    sequenceResponse toHoldGestResponse;
    toHoldGestResponse.responseName = "Gesture to Hold Gesture X";
    toHoldGestResponse.responseType = ResponseType::STATE_CHANGE;
    toHoldGestResponse.responseAction.mode = midasMode::GESTURE_HOLD_ONE;

    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::fingersSpread;
    toHoldGestResponse.responseAction.mode = midasMode::GESTURE_HOLD_TWO;
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::fist;
    toHoldGestResponse.responseAction.mode = midasMode::GESTURE_HOLD_THREE;
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::waveIn;
    toHoldGestResponse.responseAction.mode = midasMode::GESTURE_HOLD_FOUR;
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    toHoldGestSeq[0].type = Pose::Type::waveOut;
    toHoldGestResponse.responseAction.mode = midasMode::GESTURE_HOLD_FIVE;
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse);

    // Don't check ss status, as its quite possible that one of these will be in conflict. THis section is 
    // just intended to register up to five gesture hold sequences.

    // Register sequences back to Gesture Mode from Gesture Hold Modes

    sequence fromHoldGestSeq;
    fromHoldGestSeq.push_back(seqElement(Pose::Type::rest));
    sequenceResponse fromHoldGestResponse;
    fromHoldGestResponse.responseName = "Gesture from Hold Gesture X";
    fromHoldGestResponse.responseType = ResponseType::STATE_CHANGE;
    fromHoldGestResponse.responseAction.mode = midasMode::GESTURE_MODE;

    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_ONE, fromHoldGestSeq, fromHoldGestResponse);
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_TWO, fromHoldGestSeq, fromHoldGestResponse);
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_THREE, fromHoldGestSeq, fromHoldGestResponse);
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_FOUR, fromHoldGestSeq, fromHoldGestResponse);
    ss = gestSeqRecorder.registerSequence(midasMode::GESTURE_HOLD_FIVE, fromHoldGestSeq, fromHoldGestResponse);
}

void GestureFilter::handleStateChange(sequenceResponse response)
{
    if (response.responseType != ResponseType::STATE_CHANGE)
    {
        // Should NOT have made it here
        return;
    }

    std::cout << "Transitioning to state: " << response.responseAction.mode << std::endl;
    controlStateHandle->setMode(response.responseAction.mode);
    
    return;
}

void GestureFilter::handleMouseCommand(sequenceResponse response)
{
    if (controlStateHandle->getMode() == midasMode::MOUSE_MODE ||
        controlStateHandle->getMode() == midasMode::GESTURE_MODE)
    {
        filterDataMap outputToSharedCommandData;
        commandData command;
        command.type = MOUSE_COMMAND;
        command.mouse = response.responseAction.mouse;

        outputToSharedCommandData[COMMAND_INPUT] = command;
        Filter::setOutput(outputToSharedCommandData);
    }
}

void GestureFilter::handleKybrdCommand(sequenceResponse response)
{
    if (controlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        filterDataMap outputToSharedCommandData;
        commandData command;
        command.type = KEYBOARD_COMMAND;
        command.kbd = response.responseAction.kybd;

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

        sequenceResponse response;
        gf->getGestureSeqRecorder()->progressSequenceTime(SLEEP_LEN, response);
        if (response.responseType == ResponseType::STATE_CHANGE)
        {
            GestureFilter::handleStateChange(response);
        }
        //else if (response.responseType == ResponseType::MOUSE_CMD)
        //{
        //    handleMouseCommand(response);
        //}
        //else if (response.responseType == ResponseType::KYBRD_CMD)
        //{
        //    handleKybrdCommand(response);
        //}
        // TODO - figure out if this is necessary here... cant make work easily as functions
        // cant simply be converted to static. But... probably should make it work... Need to figure out.

    } while (true);
}
