#include "GestureHoldModeAction.h"

GestureHoldModeAction::GestureHoldModeAction() : minIntervalLen(50) {

}

GestureHoldModeAction::GestureHoldModeAction(unsigned int minIntervalLen) : minIntervalLen(minIntervalLen) {

}

bool GestureHoldModeAction::addToActionMap(angleData ad, kybdCmds command)
{
    commandData cd;
    cd.type = commandType::KYBRD_CMD;
    cd.action.kybd = command;

    actionMap[ad.hash()] = cd;

    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        return false; // something went wrong.
    }
    return true;
}

commandData GestureHoldModeAction::getAction(angleData ad)
{
    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        commandData defaultRet;
        defaultRet.type = commandType::NONE;
    }

    return actionMap[ad.hash()];
}