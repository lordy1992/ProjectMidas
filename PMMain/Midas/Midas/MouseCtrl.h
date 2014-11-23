#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include "MidasCommon.h"

#define DEFAULT_MIN_MOVE_TIME 10
#define DEFAULT_SCROLL_RATE 1 //Default to slow rate.
#define SCROLL_MIN_TIME 5
#define MAX_MOVE_TIME_DELTA 40 //large enough ms delay between moving a pixel is pretty slow.
#define MIN_MOVE_TIME_DELTA 1 //small enough ms delay between moving a pixel is fast, but not uncontrolled..
#define MOVE_RATE_DEADZONE 10

/**
 * Handles sending mouse data to Windows.
 */
class MouseCtrl
{
public:
    /* Basic Construction/Destruction */
    MouseCtrl();
    ~MouseCtrl();

    /**
     * Sets the scroll rate to control speed and direction.
     *
     * @param rate The rate of scrolling, between -120 and 120. A negative
     * value represents a downward scroll, positive is upward.
     */
    void setScrollRate(int rate);

    /**
     * Sets the rate at which the cursor should move in the X axis.
     *
     * @param rate The rate of cursor movement in the X axis, between
     * -100 and 100.
     */
    void setMinMoveXTimeDelta(unsigned int rate);

    /**
     * Sets the rate at which the cursor should move in the Y axis.
     *
     * @param rate The rate of cursor movement in the Y axis, between -100 and 100.
     */
    void setMinMoveYTimeDelta(unsigned int rate);

    /**
     * Sends a mouse command to the OS. This includes movement commands. If releaseIfClick is true,
     * any click event will be followed by a release of the same button. The mouseRateIfMove parameter
     * sets the rate of the mouse movement if it is nonnegative. If the user would like to "Click and Hold",
     * the releaseIfClick parameter should be set to false; then, sendCommand can be called again with 
     * releaseIfClick set to true, to release the click.
     *
     * @param mouseCmd The mouse command to send.
     * @param releaseIfClick If this is true, any button clicks are followed by a release.
     * @param mouseRateIfMove The new rate of the mouse movement.
     */
    void sendCommand(mouseCmds mouseCmd, bool releaseIfClick = true, int mouseRateIfMove = 0);

private:

    /**
     * Sets the fields of the MOUSEINPUT that will be sent to Windows.
     * @param mouseCmd The mouse command to be executed
     * @param mouseRateIfMove The rate from -100 to 100 a mouse should move.
     * This determines velocity. This value gets normalized to absolute value.
     */
    void setMouseInputVars(mouseCmds mouseCmd, int& mouseRateIfMove);

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