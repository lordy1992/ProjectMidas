#include "GestureHoldModeAction.h"

GestureHoldModeAction::GestureHoldModeAction() : minIntervalLen(50) {

}

GestureHoldModeAction::GestureHoldModeAction(unsigned int minIntervalLen) : minIntervalLen(minIntervalLen) {

}

void GestureHoldModeAction::clearMap()
{
    actionMap.clear();
}

bool GestureHoldModeAction::addToActionMap(angleData ad, kybdCmds command)
{
    actionMap[ad.hash()] = command;

    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        return false; // something went wrong.
    }
    return true;
}

kybdCmds GestureHoldModeAction::getAction(angleData ad)
{
    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        return kybdCmds::NO_CMD;
    }

    return actionMap[ad.hash()];
}