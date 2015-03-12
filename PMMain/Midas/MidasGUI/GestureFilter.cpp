#include "GestureFilter.h"
#include "MyoCommon.h"
#include "ProfileManager.h"
#include <time.h>
#include <thread>
#include <qtranslator.h>


ControlState* GestureFilter::controlStateHandle;
GestureSignaller GestureFilter::signaller;

GestureFilter::GestureFilter(ControlState* controlState, clock_t timeDel, MainGUI *mainGuiHandle) 
    : timeDelta(timeDel), lastPoseType(Pose::rest),
    lastTime(0), mainGui(mainGuiHandle)
{
    imageManager.loadImages();
    gestSeqRecorder = new GestureSeqRecorder(controlState, mainGuiHandle, imageManager);

    registerMouseSequences();
    registerKeyboardSequences();
    registerStateSequences();

    controlStateHandle = controlState;

    setupCallbackThread(this);

    if (mainGui)
    {
        mainGui->connectSignallerToInfoIndicator(&signaller);
        mainGui->connectSignallerToPoseDisplayer(&signaller);
        mainGui->connectSignallerToKeyboardToggle(&signaller);
    }

    signaller.emitStateString(QTranslator::tr((modeToString(controlState->getMode())).c_str()));
    emitPoseData(Pose::rest);
}

GestureFilter::~GestureFilter()
{
    delete gestSeqRecorder; gestSeqRecorder = NULL;
}

void GestureFilter::process()
{
    clock_t timeFromLastPose = 0;
    filterDataMap input = Filter::getInput();
    Pose::Type gesture = boost::any_cast<Pose::Type>(input[GESTURE_INPUT]);
    emitPoseData(gesture);
    
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
    ss = gestSeqRecorder->progressSequence(gesture, *controlStateHandle, response);
    
    if (response.type == commandType::NONE)
    {
        // terminate filter pipeline, as nothing to add
        Filter::setFilterStatus(filterStatus::END_CHAIN);
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

void GestureFilter::emitPoseData(int poseInt)
{
    std::vector<int> ids;
    ids.push_back(poseInt);

    std::vector<sequenceImageSet> images = imageManager.formSequenceSetFromIds(ids);

    if (images.size() == 1)
    {
        signaller.emitPoseImages(images);
    }
}

void GestureFilter::registerMouseSequences(void)
{
    // Register sequence to left click in mouse mode and gesture mode
    sequence clickSeq;
    clickSeq.push_back(SeqElement(MYO_GESTURE_LEFT_MOUSE, SeqElement::PoseLength::IMMEDIATE));
    commandData clickResp;
    clickResp.name = "Left Click";
    clickResp.type = commandType::MOUSE_CMD;
    clickResp.action.mouse = mouseCmds::LEFT_HOLD;
    int ss = (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Left Click");
    // setup clicking in gesture Mode
    clickResp.action.mouse = mouseCmds::LEFT_CLICK;
    clickSeq.at(0).poseLen = SeqElement::PoseLength::TAP; 
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp, "Left Click");

    // Register sequence to right click in mouse mode and gesture mode
    clickSeq.clear();
    clickSeq.push_back(SeqElement(MYO_GESTURE_RIGHT_MOUSE, SeqElement::PoseLength::IMMEDIATE));
    clickResp.name = "Right Click";
    clickResp.type = commandType::MOUSE_CMD;
    clickResp.action.mouse = mouseCmds::RIGHT_HOLD;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Right Click");
    // setup clicking in gesture Mode
    clickResp.action.mouse = mouseCmds::RIGHT_CLICK;
    clickSeq.at(0).poseLen = SeqElement::PoseLength::TAP;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp, "Right Click");

    // allow clicking and dragging of any button by releasing mouse buttons on rest (immediate still).
    clickResp.action.mouse = mouseCmds::RELEASE_LRM_BUTS;
    clickResp.name = "Release Mouse";
    clickSeq.at(0) = SeqElement(Pose::rest, SeqElement::PoseLength::IMMEDIATE);
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Release Mouse");

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
    int ss = (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Swap Ring Focus");
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
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Backspace");

    // register the rest of the keyboard commands
    kybrdGUIResponse.name = "Select";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::SELECT;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Select");

    kybrdGUIResponse.name = "Hold Select";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::HOLD_SELECT;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist, SeqElement::PoseLength::HOLD));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Hold Select");

    kybrdGUIResponse.name = "Change Wheels";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::CHANGE_WHEELS;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fingersSpread));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Change Wheels");

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

    int ss = (int)gestSeqRecorder->registerSequence(midasMode::LOCK_MODE, lockToMouseSeq, lockToMouseResponse, "Unlock");

    // Register sequence from Mouse Mode to Gesture Mode
    sequence mouseToGestSeq;
    mouseToGestSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    commandData mouseToGestResponse;
    mouseToGestResponse.name = "Mouse To Gesture";
    mouseToGestResponse.type = commandType::STATE_CHANGE;
    mouseToGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, mouseToGestSeq, mouseToGestResponse, "Mouse to Gesture");

    // Register sequence from Mouse Mode to Keyboard Mode
    sequence mouseToKybrdSeq;
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveOut));
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveIn));
    commandData mouseToKybrdResponse;
    mouseToKybrdResponse.name = "Mouse To Keyboard";
    mouseToKybrdResponse.type = commandType::STATE_CHANGE;
    mouseToKybrdResponse.action.mode = midasMode::KEYBOARD_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, mouseToKybrdSeq, mouseToKybrdResponse, "Mouse to Keyboard");

    // Register sequence from Gesture Mode to Mouse Mode
    sequence gestureToMouseSeq;
    gestureToMouseSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    commandData gestureToMouseResponse;
    gestureToMouseResponse.name = "Gesture To Mouse";
    gestureToMouseResponse.type = commandType::STATE_CHANGE;
    gestureToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, gestureToMouseSeq, gestureToMouseResponse, "Gesture to Mouse");

    // Register sequence from Keyboard Mode to Mouse Mode
    sequence kybrdToMouseSeq;
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::waveOut));
    commandData kybrdToMouseResponse;
    kybrdToMouseResponse.name = "Keyboard To Mouse";
    kybrdToMouseResponse.type = commandType::STATE_CHANGE;
    kybrdToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdToMouseSeq, kybrdToMouseResponse, "Keyboard to Mouse");

    // Register sequence from most Modes to Lock Mode.
    sequence toLockSeq;
    toLockSeq.push_back(SeqElement(Pose::Type::waveIn));
    toLockSeq.push_back(SeqElement(Pose::Type::thumbToPinky));
    commandData toLockResponse;
    toLockResponse.type = commandType::STATE_CHANGE;
    toLockResponse.action.mode = midasMode::LOCK_MODE;
    // From Mouse:
    toLockResponse.name = "Mouse To Lock";
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, toLockSeq, toLockResponse, "Mouse to Lock");
    // From Gesture:
    toLockResponse.name = "Gesture To Lock";
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toLockSeq, toLockResponse, "Gesture to Lock");
    // From Keyboard:
    toLockResponse.name = "Keyboard To Lock"; // OVERRIDE for Keyboard.
    toLockSeq[0] = (SeqElement(Pose::Type::thumbToPinky));
    toLockSeq[1] = (SeqElement(Pose::Type::waveIn));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, toLockSeq, toLockResponse, "Keyboard to Lock");

    // Register sequence from Gesture Mode to Gesture Hold Modes
    sequence toHoldGestSeq;
    toHoldGestSeq.push_back(SeqElement(Pose::Type::thumbToPinky, SeqElement::PoseLength::HOLD));
    commandData toHoldGestResponse;
    toHoldGestResponse.name = "Gesture to Hold Gesture X";
    toHoldGestResponse.type = commandType::STATE_CHANGE;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_ONE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Thumb to Pinky");

    toHoldGestSeq[0].type = Pose::Type::fingersSpread;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_TWO;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Fingers Spread");

    toHoldGestSeq[0].type = Pose::Type::fist;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_THREE;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Fist");

    toHoldGestSeq[0].type = Pose::Type::waveIn;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FOUR;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Wave In");

    toHoldGestSeq[0].type = Pose::Type::waveOut;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FIVE;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Wave Out");

    // Register sequences back to Gesture Mode from Gesture Hold Modes

    sequence fromHoldGestSeq;
    fromHoldGestSeq.push_back(SeqElement(Pose::Type::rest, SeqElement::PoseLength::IMMEDIATE));
    commandData fromHoldGestResponse;
    fromHoldGestResponse.name = "Gesture from Hold Gesture X";
    fromHoldGestResponse.type = commandType::STATE_CHANGE;
    fromHoldGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_ONE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Thumb to Pinky");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_TWO, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Fingers Spread");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_THREE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Fist");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FOUR, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Wave In");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FIVE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Wave Out");

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

    if (response.action.mode == midasMode::KEYBOARD_MODE || controlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        signaller.emitToggleKeyboard();
    }

    signaller.emitStateString(QTranslator::tr((modeToString(response.action.mode)).c_str()));

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

void GestureFilter::handleKybrdCommand(commandData response, bool addToExtra)
{
    if (controlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        filterDataMap outputToSharedCommandData;
        commandData command;
        command = response;

        outputToSharedCommandData[COMMAND_INPUT] = command;

        if (addToExtra) 
        {
            extraDataForSCD = outputToSharedCommandData;
        }
        else
        {
            Filter::setOutput(outputToSharedCommandData);
        }
    }
}

filterDataMap GestureFilter::getExtraDataForSCD()
{
    filterDataMap retVal = extraDataForSCD;
    extraDataForSCD.clear();
    return retVal;
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
        else if (response.type == commandType::MOUSE_CMD)
        {
            gf->handleMouseCommand(response);
        }
        else if (response.type == commandType::KYBRD_CMD)
        {
            gf->handleKybrdCommand(response, true);
        }
        else if (response.type == commandType::KYBRD_GUI_CMD)
        {
            gf->handleKybrdCommand(response, true);
        }
    } while (true);
}

void GestureFilter::dynamicallyRegisterSequences(void)
{
    gestSeqRecorder->unregisterAll();

    ProfileManager pm;
    pm.loadProfilesFromFile("TODO - THIS NEEDS A CONSTANT FILE NAME");
    std::vector<profile>* profiles = pm.getProfiles();

    int ss = (int)SequenceStatus::SUCCESS;
    for (std::vector<profile>::iterator it = profiles->begin(); it != profiles->end(); ++it)
    {
        sequence seq;
        /* for ... seq.push_back(SeqElement(Pose::Type::XXX), XXXLENXXX)*/
        /* */
        commandData command;
        command.name = "XXXName";
        command.type = commandType::STATE_CHANGE; // XXXCOMMAND_TYPE TODO
        command.action.mode = midasMode::MOUSE_MODE; // XXXMIDAS_MODE TODO

        midasMode tempXXX = midasMode::LOCK_MODE;
        ss | (int)gestSeqRecorder->registerSequence(tempXXX, seq, command, "XXXSEQUENCENAME");

        if (ss != (int)SequenceStatus::SUCCESS)
        {
            throw new std::exception("registerSequenceException: XXXSEQUENCENAME");
        }
    }
}