#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include "MidasCommon.h"

#define DEFAULT_MIN_MOVE_TIME 10
#define DEFAULT_SCROLL_RATE 1 //Default to slow rate.
#define SCROLL_MIN_TIME 5
#define MAX_MOVE_TIME_DELTA 30 //large enough ms delay between moving a pixel is pretty slow.
#define MIN_MOVE_TIME_DELTA 1 //small enough ms delay between moving a pixel is fast, but not uncontrolled..
#define MOVE_RATE_DEADZONE 5

class MouseCtrl
{
public:
    MouseCtrl();

    // set Scroll Rate to control speed/direction.
    void setScrollRate(int rate);

    // value from -100 to 100 dictating velocity X should move.
    void setMinMoveXTimeDelta(unsigned int rate);

    // value from -100 to 100 dictating velocity Y should move.
    void setMinMoveYTimeDelta(unsigned int rate);

    /*
    * Use Note: If desired to click and hold, simply set "releaseIfClick" to false. THEN,
    * to stop holding, call this function again with the same mouse click, but set "releaseIfClick"
    * to true.
    */
    void sendCommand(mouseCmds mouseCmd, bool releaseIfClick = true, int mouseRateIfMove = -1);

private:

    void setMouseInputVars(mouseCmds mouseCmd);

    MOUSEINPUT mi;
    DWORD lastMouseMoveX;
    DWORD lastMouseMoveY;
    DWORD lastMouseScroll;

    // Min time that must pass until x/y of cursor can move one pixel.
    // This is dictated by moveX/YRate, as rates are easier to conceptualize
    // for caller.
    DWORD minMoveXTimeDelta;
    DWORD minMoveYTimeDelta;

    // value from -WHEEL_DELTA to WHEEL_DELTA dictating portion of a scroll click
    // will happen when the "scrollwheel is moved"
    int scrollRate;
};

#endif /* _MOUSE_CONTROLLER_H */