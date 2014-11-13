#include "MouseCtrl.h"
#include <time.h>
#include <iostream>

MouseCtrl::MouseCtrl()
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    lastMouseMovement = clock();
    lastMouseScroll = lastMouseMovement;
    minMovementTimeDelta = 10; // default arbitrarily to 5 ms gaps.
    std::cout << "time on mouseCtrlCreate = " << lastMouseMovement << std::endl;
}

void MouseCtrl::setMinMovementTimeDelta(time_t delta)
{
    minMovementTimeDelta = delta;
}

void MouseCtrl::setScrollRate(int rate)
{
    if (rate < -120 || rate > 120)
    {
        scrollRate = DEFAULT_SCROLL_RATE; 
    } 
    else
    {
        scrollRate = rate;
    }
}

/*
 * Use Note: If desired to click and hold, simply set "releaseIfClick" to false. THEN,
 * to stop holding, call this function again with the same mouse click, but set "releaseIfClick"
 * to true.
 */
void MouseCtrl::sendCommand(mouseCmds mouseCmd, bool releaseIfClick)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    time_t currentTime = clock();
    time_t deltaTimeMove = currentTime - lastMouseMovement;
    time_t deltaTimeScroll = currentTime - lastMouseScroll;

    switch (mouseCmd)
    {
    case mouseCmds::LEFT_CLICK:
        mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case mouseCmds::RIGHT_CLICK:
        mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case mouseCmds::MIDDLE_CLICK:
        mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    case mouseCmds::MOVE_LEFT:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = -1;
        break;
    case mouseCmds::MOVE_RIGHT:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = 1;
        break;
    case mouseCmds::MOVE_UP:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = -1; // negative moves up.
        break;
    case mouseCmds::MOVE_DOWN:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = 1;
        break;
    case mouseCmds::SCROLL_LEFT:
        //TODO - Not working
        mi.dwFlags = MOUSEEVENTF_HWHEEL;
        mi.mouseData = -scrollRate;
        break;
    case mouseCmds::SCROLL_RIGHT:
        //TODO - Not working
        mi.dwFlags = MOUSEEVENTF_HWHEEL;
        mi.mouseData = scrollRate;
        break;
    case mouseCmds::SCROLL_UP:
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = scrollRate; // RANGE IS FROM -120 to +120 - WHEEL_DELTA = 120, which is one "wheel click"
        break;
    case mouseCmds::SCROLL_DOWN:
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = -scrollRate; // RANGE IS FROM -120 to +120 - WHEEL_DELTA = 120, which is one "wheel click"
        break;
    }

    if (deltaTimeMove < minMovementTimeDelta &&
        mi.dwFlags == MOUSEEVENTF_MOVE)
    {
        // Not enough time has passed to move the mouse again
        return;
    }
    else
    {
        lastMouseMovement = currentTime;
    }

    if (deltaTimeScroll < SCROLL_MIN_TIME &&
        (mi.dwFlags == MOUSEEVENTF_WHEEL ||
        mi.dwFlags == MOUSEEVENTF_HWHEEL))
    {
        // Not enough time has passed to move the mouse again
        return;
    } 
    else
    {
        lastMouseScroll = currentTime;
    }

    INPUT* in = new INPUT();
    in->type = INPUT_MOUSE;
    in->mi = mi;
    SendInput(1, in, sizeof(INPUT));

    if (releaseIfClick) 
    {
        ZeroMemory(&mi, sizeof(MOUSEINPUT));
        switch (mouseCmd)
        {
        case mouseCmds::LEFT_CLICK:
            mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case mouseCmds::RIGHT_CLICK:
            mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case mouseCmds::MIDDLE_CLICK:
            mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            goto done;
        }

        in->type = INPUT_MOUSE;
        in->mi = mi;
        SendInput(1, in, sizeof(INPUT));
    }

    done:
    delete in; in = NULL;
}