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
    if (scdHandle->consumeCommand(nextCmd))
    {
        if (nextCmd.mouse == LEFT_CLICK)
        {
            std::cout << "Received a left click." << std::endl;
            mouseCtrl->sendCommand(mouseCmds::LEFT_CLICK, false);
        }
        else if (nextCmd.mouse == RIGHT_CLICK)
        {
            std::cout << "Received a right click." << std::endl;
            mouseCtrl->sendCommand(mouseCmds::RIGHT_CLICK, false);
        }
        else if (nextCmd.mouse == LEFT_RELEASE)
        {
            std::cout << "Received a left release." << std::endl;
            mouseCtrl->sendCommand(mouseCmds::LEFT_CLICK, true);
        }
        else if (nextCmd.mouse == RIGHT_RELEASE)
        {
            std::cout << "Received a right release." << std::endl;
            mouseCtrl->sendCommand(mouseCmds::RIGHT_CLICK, true);
        }
    }

    point unitVelocity = scdHandle->getVelocity();
    if (unitVelocity.x != 0)
    {
        mouseCtrl->sendCommand(mouseCmds::MOVE_HOR, true, unitVelocity.x);
        if (count % 1000 == 0)
        {
            // proof of concept - slowed down as to reduce buildup in signal buffer...
            threadHandle->emitXVeloc(unitVelocity.x);
        }
    }
    if (unitVelocity.y != 0)
    {
        mouseCtrl->sendCommand(mouseCmds::MOVE_VERT, true, unitVelocity.y);
        if (count % 1000 == 0)
        {
            // proof of concept - slowed down as to reduce buildup in signal buffer...
            threadHandle->emitYVeloc(unitVelocity.y);
        }
    }

    float deltaVolume = scdHandle->getDeltaVolume();
    if (count % 10000 == 0)
    {
        if (deltaVolume > 0)
        {
            threadHandle->threadEmitString("volume up" + std::to_string(count));
            kybrdCtrl->setKeyCmd(kybdCmds::VOLUME_UP);
            kybrdCtrl->sendData();
        }
        else if (deltaVolume < 0)
        {
            threadHandle->threadEmitString("volume down" + std::to_string(count));
            kybrdCtrl->setKeyCmd(kybdCmds::VOLUME_DOWN);
            kybrdCtrl->sendData();
        }
    }

    if (count % 100000 == 0)
    {
        //threadHandle->threadEmitString(std::to_string(count)); // this proves we can modify gui from here! woot.
        threadHandle->threadEmitStateString(std::to_string(cntrlStateHandle->getMode()));
        
        std::cout << "Percent of X: " << unitVelocity.x << ", Percent of Y: " << unitVelocity.y << std::endl;
    }
    count++;
}
