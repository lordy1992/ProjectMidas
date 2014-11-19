#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include "MidasCommon.h"

#define DEFAULT_MIN_MOVE_TIME 10
#define DEFAULT_SCROLL_RATE 1 //Default to slow rate.
#define SCROLL_MIN_TIME 5
#define MAX_MOVE_TIME_DELTA 40 //large enough ms delay between moving a pixel is pretty slow.
#define MIN_MOVE_TIME_DELTA 1 //small enough ms delay between moving a pixel is fast, but not uncontrolled..
#define MOVE_RATE_DEADZONE 5

class MouseCtrl
{
public:
    /* Basic Construction/Destruction */
    MouseCtrl();
    ~MouseCtrl();

    /*
     * Input: 
     *      rate - value from -WHEEL_DELTA to WHEEL_DELTA dictating portion of a scroll click
     * will happen when the "scrollwheel is moved"
     * Purpose: 
     *      set Scroll Rate to control speed/direction.
     */
    void setScrollRate(int rate);

    /* 
     * Input: 
     *      rate - value from -100 to 100 dictating velocity X should move.
     * Purpose: 
     *      set minMoveXTimeDelta
     */
    void setMinMoveXTimeDelta(unsigned int rate);

    /*
    * Input: 
    *       rate - value from -100 to 100 dictating velocity Y should move.
    * Purpose:
    *       set minMoveYTimeDelta
    */
    void setMinMoveYTimeDelta(unsigned int rate);

    /*
     * Inputs:
     *      mouseCmd - the mouse command to be executed
     *      releaseIfClick - bool dictating that if the cmd is a click, it will also release
     *      mosueRateIfMove - int that if >= 0 and <= 100 sets the rate at which the mouse will move
     * Purpose:
     *      Configure the mouse command to be executed and have the command executed.
     * Use Note: If desired to click and hold, simply set "releaseIfClick" to false. THEN,
     * to stop holding, call this function again with the same mouse click, but set "releaseIfClick"
     * to true.
     */
    void sendCommand(mouseCmds mouseCmd, bool releaseIfClick = true, int mouseRateIfMove = -1);

private:

    /*
     * Inputs:
     *      mouseCmd - the mouse command to be executed
     * Purpose:
     *      Helper function to configure the MOUSEINPUT values in a modularized fashion
     */
    void setMouseInputVars(mouseCmds mouseCmd);

    // Mouse Input 
    MOUSEINPUT mi;

    // Time Stamps used to limit move and scroll speed.
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