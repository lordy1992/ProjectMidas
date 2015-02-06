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
    count = 0;
}


SCDDigester::~SCDDigester()
{
}

void SCDDigester::digest()
{
 //   threadHandle->emitUpdateKeyboard(1, 210, false, false); // TODO TEMP

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
        keyboardAngle currAngle = scdHandle->getKeySelectAngle();

        if (count % 1000 == 0)
        {
            double angleAsDouble = (double) currAngle.angle;
            threadHandle->emitKeyboardData(kybdGUISel, angleAsDouble);
        }

       // digestKeyboardData(nextCmd);
    }

    count++;
}

void SCDDigester::digestKeyboardData(commandData nextCommand)
{
    keyboardAngle currAngle;
    int ringSegmentAngle, ringKeySelIdx;
    char key;
    if (nextCommand.type == KYBRD_CMD)
    {
        // handle as regular keyboard command input
        kybrdCtrl->setKeyCmd(nextCommand.action.kybd);
        kybrdCtrl->sendData();
    }
    else if (nextCommand.type == KYBRD_GUI_CMD)
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
            ringSegmentAngle = 360 / (*kybrdRingData)[kybdGUISel].getRingInVectorHandle()->size();
            ringKeySelIdx = floor(currAngle.angle / ringSegmentAngle);

            key = (*kybrdRingData)[kybdGUISel].getRingInVectorHandle()->at(ringKeySelIdx).main;

            kybrdCtrl->setKeyChar(key);
            kybrdCtrl->sendData();

            threadHandle->emitUpdateKeyboard(kybdGUISel, 30, false, false);

            /* Todo, pseudocode written 
            1) pass this character to the keyboard as such
                kybrdCtrl->setKeyChar(charThatWasDetermined);
                kybrdCtrl->sendData();    
            2) tell the GUI that this was selected and that it should play some sort of animation *small flash or something, etc*
            */
            
            break;
        case kybdGUICmds::HOLD_SELECT:
            /* Todo, pseudocode written
            EXACT same thing as select except that the HOLD character is used, rather than the regular character.
            // in the discusion of RingData, this would correspond to the "*Hold vectors"
            */
            currAngle = scdHandle->getKeySelectAngle();
            ringSegmentAngle = 360 / (*kybrdRingData)[kybdGUISel].getRingInVectorHandle()->size();
            ringKeySelIdx = floor(currAngle.angle / ringSegmentAngle);

            key = (*kybrdRingData)[kybdGUISel].getRingInVectorHandle()->at(ringKeySelIdx).hold;

            kybrdCtrl->setKeyChar(key);
            kybrdCtrl->sendData();

            threadHandle->emitUpdateKeyboard(kybdGUISel, 30, false, true);

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