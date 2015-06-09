#include "SCDDigester.h"


SCDDigester::SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, 
    MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl)
{
    this->scdHandle = scd;
    this->threadHandle = thread;
    this->cntrlStateHandle = cntrlStateHandle;
    this->mouseCtrl = mouseCtrl;
    this->kybrdCtrl = kybrdCtrl;
    count = 0;
}


SCDDigester::~SCDDigester()
{
}

void SCDDigester::digest()
{
    commandData nextCmd;
    static bool isConnected = true;
    static bool testConnected = true;

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

        /* Only update GUI if the connection status changed*/
        testConnected = scdHandle->getIsConnected();
        if (isConnected != testConnected)
        {
            if (!testConnected)
            {
                threadHandle->emitDisconnect(testConnected);
            }
        }
        isConnected = testConnected;

        /* Only update RSSI if device connected */
        if (isConnected)
        {
            float rssi = scdHandle->getRssi();
            threadHandle->emitRssi(rssi);
        }

    }

    if (cntrlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        //unsigned int kybdGUISel = scdHandle->getKybdGuiSel();
        //keyboardAngle currKeyAngle = scdHandle->getKeySelectAngle();
		//
        //if (count % 1000 == 0)
        //{
        //    double angleAsDouble = (double)currKeyAngle.angle;
        //    threadHandle->emitUpdateKeyboard(kybdGUISel, angleAsDouble, currKeyAngle.ringThreshReached, false);
		//
        //    // // TEMP TODO for debug only
        //    // int x = currKeyAngle.x;
        //    // int y = currKeyAngle.y;
        //    // threadHandle->emitDebugInfo(x, y);
        //}
		//
        //digestKeyboardGUIData(nextCmd);
    }
    
    count++;
}

void SCDDigester::digestKeyboardGUIData(commandData nextCommand)
{
    /*keyboardAngle currAngle;
    int ringKeySelIdx;
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
            currAngle = scdHandle->getKeySelectAngle();

            if (currAngle.ringThreshReached)
            {
                if (kybdGUISel % 2 == 0)
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle()->at(ringKeySelIdx).main;
                }
                else
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle()->at(ringKeySelIdx).main;
                }
            }
            else
            {
                key = CENTER_MAIN_KEY;
            }

            kybrdCtrl->setKeyChar(key);
            kybrdCtrl->sendData();

            //threadHandle->animateSelection(); // TODO
            
            break;
        case kybdGUICmds::HOLD_SELECT:
            currAngle = scdHandle->getKeySelectAngle();

            if (currAngle.ringThreshReached)
            {
                if (kybdGUISel % 2 == 0)
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle()->at(ringKeySelIdx).hold;
                }
                else
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle()->at(ringKeySelIdx).hold;
                }
            }
            else
            {
                key = CENTER_HOLD_KEY;
            }

            kybrdCtrl->setKeyChar(key);
            kybrdCtrl->sendData();

            //threadHandle->animateSelection(); // TODO

            break;
        default:
            break;
        }
    }*/
}

void SCDDigester::digestKybdCmd(commandData nextCommand)
{
    kybrdCtrl->setKeyCmd(nextCommand.action.kybd);
    kybrdCtrl->sendData();
}
