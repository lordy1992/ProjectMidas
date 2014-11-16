#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include "MidasCommon.h"

#define DEFAULT_MIN_MOVE_TIME 10
#define DEFAULT_SCROLL_RATE 1 //Default to slow rate.
#define SCROLL_MIN_TIME 5

class MouseCtrl
{
public:
    MouseCtrl();

    /*
    */
    void setMinMovementTimeDelta(DWORD delta);

    void setScrollRate(int rate);

    /*
    * Use Note: If desired to click and hold, simply set "releaseIfClick" to false. THEN,
    * to stop holding, call this function again with the same mouse click, but set "releaseIfClick"
    * to true.
    */
    void sendCommand(mouseCmds mouseCmd, bool releaseIfClick = true);

private:
    MOUSEINPUT mi;
    DWORD lastMouseMovement;
    DWORD lastMouseScroll;
    DWORD minMovementTimeDelta;
    int scrollRate;
};

#endif /* _MOUSE_CONTROLLER_H */