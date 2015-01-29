#include "SCDDigester.h"


SCDDigester::SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl)
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
        if (count % 100 == 0)
        {
            threadHandle->emitVeloc(unitVelocity.x, unitVelocity.y);
        }
    }
    if (unitVelocity.y != 0)
    {
        mouseCtrl->sendCommand(mouseCmds::MOVE_VERT, unitVelocity.y);
        if (count % 100 == 0)
        {
            threadHandle->emitVeloc(unitVelocity.x, unitVelocity.y);
        }
    }

    if (cntrlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        digestKeyboardData(nextCmd);
    }

    count++;
}

void SCDDigester::digestKeyboardData(commandData nextCommand)
{
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
            if (kybdGUISel % 4 == 0)
            {
                kybdGUISel += 1;
            }
            else
            {
                kybdGUISel -= 1;
            }
            //threadHandle->UpdateGUIToSwapRingFocus TODO
            scdHandle->setKybdGuiSel(kybdGUISel);
            break;
        case kybdGUICmds::CHANGE_WHEELS:
            // go to next wheel
            kybdGUISel += 4;
            kybdGUISel %= (scdHandle->getKybdGuiSelMax());
            //threadHandle->UpdateGUIToChangeWheels TODO
            scdHandle->setKybdGuiSel(kybdGUISel);
            break;
        case kybdGUICmds::SELECT:
            /* Todo, pseudocode written 
            scdHandle->getAngle()
            use angle and current kybdGUISel to determine which character is being highlighted.
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