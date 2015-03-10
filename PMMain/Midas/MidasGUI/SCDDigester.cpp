#include "SCDDigester.h"


SCDDigester::SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, 
    MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl, std::vector<ringData> *kybrdRingData)
{
    this->scdHandle = scd;
    this->threadHandle = thread;
    this->cntrlStateHandle = cntrlStateHandle;
    this->mouseCtrl = mouseCtrl;
    this->kybrdCtrl = kybrdCtrl;
    this->kybrdRingData = kybrdRingData;
    this->keyboardWidget = keyboardWidget;

#ifdef VERSION2
    currKeySelect = 0;
#endif
    count = 0;
    start_kbd = clock();
}


SCDDigester::~SCDDigester()
{
}

void SCDDigester::digest()
{
    commandData nextCmd;
    
    bool consumed = scdHandle->consumeCommand(nextCmd);

    switch (nextCmd.type)
    {
    case KYBRD_CMD:
        digestKybdCmd(nextCmd);
        break;
    case KYBRD_GUI_CMD:
        break;
    case MOUSE_CMD:
        break;
    case STATE_CHANGE:
        break;
    case NONE:
        break;
    case UNKNOWN_COMMAND:
        break;
    default:
        break;
    }

    if (consumed && nextCmd.type == commandType::MOUSE_CMD)
    {
        mouseCtrl->sendCommand(nextCmd.action.mouse);
    }

    point unitVelocity = scdHandle->getVelocity();
    if (unitVelocity.x != 0)
    {
        mouseCtrl->sendCommand(mouseCmds::MOVE_HOR, unitVelocity.x);

    }
    if (unitVelocity.y != 0)
    {
        mouseCtrl->sendCommand(mouseCmds::MOVE_VERT, unitVelocity.y);
    }

    if (count % 1000 == 0)
    {
        threadHandle->emitVeloc(unitVelocity.x, unitVelocity.y);
    }

    if (cntrlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        unsigned int kybdGUISel = scdHandle->getKybdGuiSel();
        keyboardAngle currKeyAngle = scdHandle->getKeySelectAngle();
        int ringKeySelIdx;
        ringData::keyboardValue selRing;
        ringData ringDat = kybrdRingData->at(kybdGUISel / 2);
        bool centerSelect = true;
#ifdef VERSION2

        centerSelect = false;
        std::vector<ringData::keyboardValue> *whichRing;
        if (kybdGUISel % 2 == 0)
        {
            whichRing = ringDat.getRingOutVectorHandle();
        }
        else
        {
            whichRing = ringDat.getRingInVectorHandle();
        }

        clock_t elapsed = clock() - start_kbd;
        if (elapsed > CLOCKS_PER_SEC / 4)
        {
            // 1 second latency
            if (currKeyAngle.angle > 20) 
            {
                currKeySelect++;
            }
            else if (currKeyAngle.angle < -20)
            {
                currKeySelect--;
            }

            if (currKeySelect < 0)
            {
                currKeySelect = whichRing->size();
            } 
            else if (currKeySelect > whichRing->size())
            {
                currKeySelect = 0;
            } 

            threadHandle->emitUpdateKeyboard(kybdGUISel, currKeySelect, currKeyAngle.ringThreshReached, false);
            start_kbd = clock();
        }

        if (currKeySelect > 0)
        {
            selRing = whichRing->at(currKeySelect - 1);
        }
        else
        {
            centerSelect = true;
        }
#else
        if (currKeyAngle.ringThreshReached)
        {
            centerSelect = false;
            if (kybdGUISel % 2 == 0)
            {
                ringKeySelIdx = getSelectedKeyFromAngle(currKeyAngle.angle, ringDat.getRingOutVectorHandle());
                selRing = ringDat.getRingOutVectorHandle()->at(ringKeySelIdx);
            }
            else
            {
                ringKeySelIdx = getSelectedKeyFromAngle(currKeyAngle.angle, ringDat.getRingInVectorHandle());
                selRing = ringDat.getRingInVectorHandle()->at(ringKeySelIdx);
            }
        }

        if (count % 1000 == 0)
        {
            double angleAsDouble = (double)currKeyAngle.angle;
            threadHandle->emitUpdateKeyboard(kybdGUISel, ringKeySelIdx, currKeyAngle.ringThreshReached, false);
        }
#endif
        digestKeyboardGUIData(nextCmd, selRing, centerSelect);
    }

    count++;
}

void SCDDigester::digestKeyboardGUIData(commandData nextCommand, ringData::keyboardValue selRing, bool inCenter)
{
    char key;
    if (nextCommand.type == KYBRD_GUI_CMD)
    {
        unsigned int kybdGUISel = scdHandle->getKybdGuiSel();

        // handle special commands for keyboard gui updating.
        switch (nextCommand.action.kybdGUI)
        {
        case kybdGUICmds::SWAP_RING_FOCUS:
            // Swap which ring is focussed on (out/in) 
            // based on RingData structure: adding 1 will go from outer to inner ring and sub 1 will go from inner to outer
            if (kybdGUISel % 2 == 0)
            {
                kybdGUISel += 1;
            }
            else
            {
                kybdGUISel -= 1;
            }
            scdHandle->setKybdGuiSel(kybdGUISel);
            break;

        case kybdGUICmds::CHANGE_WHEELS:
            // go to next wheel
            kybdGUISel += 2;
            kybdGUISel %= (scdHandle->getKybdGuiSelMax());
            scdHandle->setKybdGuiSel(kybdGUISel);
            break;

        case kybdGUICmds::SELECT:
            key = (inCenter) ? CENTER_MAIN_KEY : selRing.main;

            kybrdCtrl->setKeyChar(key);
            kybrdCtrl->sendData();

            break;
        case kybdGUICmds::HOLD_SELECT:
            key = (inCenter) ? CENTER_HOLD_KEY : selRing.hold;

            kybrdCtrl->setKeyChar(key);
            kybrdCtrl->sendData();

            break;
        default:
            break;
        }
    }
}

void SCDDigester::digestKybdCmd(commandData nextCommand)
{
    kybrdCtrl->setKeyCmd(nextCommand.action.kybd);
    kybrdCtrl->sendData();
}

// MAKE SURE THIS FUNCTION MATCHES THE SAME FUNCTION IN SCDDigester.
int SCDDigester::getSelectedKeyFromAngle(double angle, std::vector<ringData::keyboardValue> *ring)
{
    qreal deltaAngle = 360.0 / ring->size();
    int adjustedAngle = (int)(angle + deltaAngle / 2) % 360;

    // TODO: May have to change later, based on received angle
    return (int)(adjustedAngle / deltaAngle);
}