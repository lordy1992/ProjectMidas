#include "MouseCtrl.h"
#include <iostream>
#include <time.h>

MouseCtrl::MouseCtrl()
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    lastMouseMoveX = clock() * (1000 / CLOCKS_PER_SEC);
    lastMouseMoveY = lastMouseMoveX;
    lastMouseScroll = lastMouseMoveX;
    minMoveXTimeDelta = DEFAULT_MIN_MOVE_TIME;
    minMoveYTimeDelta = DEFAULT_MIN_MOVE_TIME;
    scrollRate = DEFAULT_SCROLL_RATE;
    //std::cout << "time on mouseCtrlCreate = " << lastMouseMovement << std::endl;
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

void MouseCtrl::setMinMoveXTimeDelta(unsigned int rate)
{
    int moveXRate = min(rate, 100);

    minMoveXTimeDelta = MAX_MOVE_TIME_DELTA - ((rate / 100.0) * MAX_MOVE_TIME_DELTA);
}

void MouseCtrl::setMinMoveYTimeDelta(unsigned int rate)
{
    int moveYRate = min(rate, 100);

    minMoveYTimeDelta = MAX_MOVE_TIME_DELTA - ((rate / 100.0) * MAX_MOVE_TIME_DELTA);
}

void MouseCtrl::sendCommand(mouseCmds mouseCmd, bool releaseIfClick, int mouseRateIfMove)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    DWORD currentTime = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD deltaTimeXMove = currentTime - lastMouseMoveX;
    DWORD deltaTimeYMove = currentTime - lastMouseMoveY;
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
        if (deltaTimeXMove < minMoveXTimeDelta)
            return;
        else
            lastMouseMoveX = currentTime;
        if (mouseRateIfMove >= 0) 
            setMinMoveXTimeDelta(mouseRateIfMove);
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = -1;
        break;
    case mouseCmds::MOVE_RIGHT:
        if (deltaTimeXMove < minMoveXTimeDelta)
            return;
        else
            lastMouseMoveX = currentTime;
        if (mouseRateIfMove >= 0) 
            setMinMoveXTimeDelta(mouseRateIfMove);
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = 1;
        break;
    case mouseCmds::MOVE_UP:
        if (deltaTimeYMove < minMoveYTimeDelta)
            return;
        else
            lastMouseMoveY = currentTime;
        if (mouseRateIfMove >= 0) 
            setMinMoveYTimeDelta(mouseRateIfMove);
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = -1; // negative moves up.
        break;
    case mouseCmds::MOVE_DOWN:
        if (deltaTimeYMove < minMoveYTimeDelta)
            return;
        else
            lastMouseMoveY = currentTime;
        if (mouseRateIfMove >= 0) 
            setMinMoveYTimeDelta(mouseRateIfMove);
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

    //TODO - assert if mi.dx != 0, then mi.dy == 0

    //if (mi.dwFlags == MOUSEEVENTF_MOVE && 
    //    ((lastMouseMoveX < minMoveXTimeDelta && mi.dx != 0) ||
    //    (lastMouseMoveY < minMoveYTimeDelta && mi.dy != 0))
    //    )
    //{
    //    // Not enough time has passed to move the mouse again
    //    return;
    //}
    //else
    //{
    //    if (mi.dx != 0)
    //    {
    //        lastMouseMoveX = currentTime;
    //    }
    //    if (mi.dy != 0)
    //    {
    //        lastMouseMoveY = currentTime;
    //    }
    //}

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