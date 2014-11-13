#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include "MidasCommon.h"

#define DEFAULT_SCROLL_RATE 1 //Default to slow rate.
#define SCROLL_MIN_TIME 5

class MouseCtrl
{
public:
    MouseCtrl();

    void setMinMovementTimeDelta(time_t delta);

    void setScrollRate(int rate);

    void sendCommand(mouseCmds mouseCmd, bool releaseIfClick = true);

private:
    MOUSEINPUT mi;
    time_t lastMouseMovement;
    time_t lastMouseScroll;
    time_t minMovementTimeDelta;
    int scrollRate;
};

#endif /* _MOUSE_CONTROLLER_H */