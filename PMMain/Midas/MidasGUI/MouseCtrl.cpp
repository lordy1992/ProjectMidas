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
}

MouseCtrl::~MouseCtrl()
{
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
//    int range = 100 - MOVE_RATE_DEADZONE; // available rate delta to deduce velocity from
//
//    int numPossibleDeltas = MAX_MOVE_TIME_DELTA - MIN_MOVE_TIME_DELTA + 1;
//    double speedUpFactor[MAX_MOVE_TIME_DELTA - MIN_MOVE_TIME_DELTA + 1];
//
//    for (int i = 0; i < numPossibleDeltas; i++)
//    {
//        // For example this array is being populated with the series 2/1, 3/2, 4/3, 5/4, ... Which indicates the TRUE factor 
//        // of speedup that each increment incurs. Thus using this data, velocity can be linearized.
//        speedUpFactor[i] = ((double)(i + MIN_MOVE_TIME_DELTA + 1) / (i + MIN_MOVE_TIME_DELTA)) - 1;
//    }
//
//    minMoveXTimeDelta = MAX_MOVE_TIME_DELTA; // default
//    double angleThresh = 1.0;// (double)MOVE_RATE_DEADZONE;
//    for (int i = 0; i < numPossibleDeltas; i++)
//    {
//        //if (rate <= angleThresh)
//        //{
//        //    minMoveXTimeDelta = max(MAX_MOVE_TIME_DELTA - i, MIN_MOVE_TIME_DELTA); // just incase, ensure that the Min Move Time Delta isn't passed.
//        //    break;
//        //}
//        //
//        //angleThresh *= speedUpFactor[numPossibleDeltas - 1 - i];
//
//        if (rate <= MOVE_RATE_DEADZONE + (range * speedUpFactor[numPossibleDeltas - 1 - i] - 1))
//        {
//            minMoveYTimeDelta = max(MAX_MOVE_TIME_DELTA - i, MIN_MOVE_TIME_DELTA); // just incase, ensure that the Min Move Time Delta isn't passed.
//            break;
//        }
//    }

    minMoveXTimeDelta = max(MAX_MOVE_TIME_DELTA - ((rate / 100.0) * MAX_MOVE_TIME_DELTA), MIN_MOVE_TIME_DELTA);
}

void MouseCtrl::setMinMoveYTimeDelta(unsigned int rate)
{
//    int range = 100 - MOVE_RATE_DEADZONE; // available rate delta to deduce velocity from
//
//    int numPossibleDeltas = MAX_MOVE_TIME_DELTA - MIN_MOVE_TIME_DELTA + 1;
//    double speedUpFactor[MAX_MOVE_TIME_DELTA - MIN_MOVE_TIME_DELTA + 1];
//
//    for (int i = 0; i < numPossibleDeltas; i++)
//    {
//        // For example this array is being populated with the series 2/1, 3/2, 4/3, 5/4, ... Which indicates the TRUE factor 
//        // of speedup that each increment incurs. Thus using this data, velocity can be linearized.
//        speedUpFactor[i] = ((double)(i + MIN_MOVE_TIME_DELTA + 1) / (i + MIN_MOVE_TIME_DELTA)) - 1;
//    }
//
//    minMoveYTimeDelta = MAX_MOVE_TIME_DELTA; // default
//    double angleThresh = 1.0;// (double)MOVE_RATE_DEADZONE;
//    for (int i = 0; i < numPossibleDeltas; i++)
//    {
//        //if (rate <= angleThresh)
//        //{
//        //    minMoveYTimeDelta = max(MAX_MOVE_TIME_DELTA - i, MIN_MOVE_TIME_DELTA); // just incase, ensure that the Min Move Time Delta isn't passed.
//        //    break;
//        //}
//        //
//        //angleThresh *= speedUpFactor[numPossibleDeltas - 1 - i];
//
//        if (rate <= MOVE_RATE_DEADZONE + (range * speedUpFactor[numPossibleDeltas - 1 - i] - 1))
//        {
//            minMoveYTimeDelta = max(MAX_MOVE_TIME_DELTA - i, MIN_MOVE_TIME_DELTA); // just incase, ensure that the Min Move Time Delta isn't passed.
//            break;
//        }
//    }


    minMoveYTimeDelta = max(MAX_MOVE_TIME_DELTA - ((rate / 100.0) * MAX_MOVE_TIME_DELTA), MIN_MOVE_TIME_DELTA);
}

void MouseCtrl::sendCommand(mouseCmds mouseCmd, bool releaseIfClick, int mouseRateIfMove)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    DWORD currentTime = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD deltaTimeXMove = currentTime - lastMouseMoveX;
    DWORD deltaTimeYMove = currentTime - lastMouseMoveY;
    DWORD deltaTimeScroll = currentTime - lastMouseScroll;


    setMouseInputVars(mouseCmd, mouseRateIfMove);

    // Handle Early exit cases if moving mouse
    if (mi.dwFlags == MOUSEEVENTF_MOVE 
        && 
        (((deltaTimeXMove < minMoveXTimeDelta && mi.dx != 0) ||
        (deltaTimeYMove < minMoveYTimeDelta && mi.dy != 0))
        || 
        (mouseRateIfMove > 0 && mouseRateIfMove < MOVE_RATE_DEADZONE)
        ||
        (mi.dx != 0 && mi.dy != 0))
        )
    {
        // Not enough time has passed to move the mouse again or 
        // in deadzone or
        // movements not mutually exclusive.
        return;
    }
    else
    {
        // Update time stamps
        if (mi.dx != 0)
        {
            if (mouseRateIfMove >= 0)
                setMinMoveXTimeDelta(mouseRateIfMove);
            lastMouseMoveX = currentTime;
        }
        if (mi.dy != 0)
        {
            if (mouseRateIfMove >= 0)
                setMinMoveYTimeDelta(mouseRateIfMove);
            lastMouseMoveY = currentTime;
        }
    }

    // Handle early exit cases if scrolling mouse
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

    // Send Command!
    INPUT* in = new INPUT();
    in->type = INPUT_MOUSE;
    in->mi = mi;
    SendInput(1, in, sizeof(INPUT));

    // Build and send opposite command if releasing a click!
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

void MouseCtrl::setMouseInputVars(mouseCmds mouseCmd, int& mouseRateIfMove)
{
    if (mouseCmd == MOVE_HOR && mouseRateIfMove < 0)
    {
        mouseCmd = MOVE_LEFT;
    }
    else if (mouseCmd == MOVE_HOR && mouseRateIfMove >= 0)
    {
        mouseCmd = MOVE_RIGHT;
    }
    else if (mouseCmd == MOVE_VERT && mouseRateIfMove < 0)
    {
        mouseCmd = MOVE_DOWN;
    }
    else if (mouseCmd == MOVE_VERT && mouseRateIfMove >= 0)
    {
        mouseCmd = MOVE_UP;
    }

    mouseRateIfMove = abs(mouseRateIfMove);

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
        mi.mouseData = scrollRate; // RANGE IS FROM -120 to +120 : WHEEL_DELTA = 120, which is one "wheel click"
        break;
    case mouseCmds::SCROLL_DOWN:
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = -scrollRate; // RANGE IS FROM -120 to +120 : WHEEL_DELTA = 120, which is one "wheel click"
        break;
    }
}