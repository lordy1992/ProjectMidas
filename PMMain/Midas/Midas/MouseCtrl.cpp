#include "MouseCtrl.h"
#include <iostream>
#include <time.h>

MouseCtrl::MouseCtrl()
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    lastMouseMovement = clock() * (1000 / CLOCKS_PER_SEC);
    lastMouseScroll = lastMouseMovement;
    minMovementTimeDelta = 1;//DEFAULT_MIN_MOVE_TIME;
    //std::cout << "time on mouseCtrlCreate = " << lastMouseMovement << std::endl;
}

void MouseCtrl::setMinMovementTimeDelta(DWORD delta)
{
    minMovementTimeDelta = delta;
}

void MouseCtrl::setScrollRate(int rate)
{
    if (rate < -WHEEL_DELTA || rate > WHEEL_DELTA)
    {
        scrollRate = DEFAULT_SCROLL_RATE; 
    } 
    else
    {
        scrollRate = rate;
    }
}

void MouseCtrl::sendCommand(mouseCmds mouseCmd, bool releaseIfClick)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    DWORD currentTime = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD deltaTimeMove = currentTime - lastMouseMovement;
    DWORD deltaTimeScroll = currentTime - lastMouseScroll;

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
        if (deltaTimeMove > 0)
        {
            //std::cout << "delta not high enough: " << deltaTimeMove << std::endl;
        }
        return;
    }
    else
    {
        lastMouseMovement = currentTime;
        //std::cout << "updating lastMouseMovement to: " << lastMouseMovement << std::endl;
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