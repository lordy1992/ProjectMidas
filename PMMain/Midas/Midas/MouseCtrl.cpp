#include "MouseCtrl.h"

mouseCtrl::mouseCtrl()
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
}

/*
 * Use Note: If desired to click and hold, simply set "releaseIfClick" to false. THEN,
 * to stop holding, call this function again with the same mouse click, but set "releaseIfClick"
 * to true.
 */
void mouseCtrl::sendCommand(mouseCmds mouseCmd, bool releaseIfClick)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));

    switch (mouseCmd)
    {
    case mouseCmds::LEFT_CLICK:
        mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    case mouseCmds::RIGHT_CLICK:
        mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    case mouseCmds::MIDDLE_CLICK:
        mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
    case mouseCmds::MOVE_LEFT:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = -1;
    case mouseCmds::MOVE_RIGHT:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = 1;
    case mouseCmds::MOVE_UP:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = -1; // negative moves up.
    case mouseCmds::MOVE_DOWN:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = 1;
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
        case mouseCmds::RIGHT_CLICK:
            mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        case mouseCmds::MIDDLE_CLICK:
            mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        }

        in->type = INPUT_MOUSE;
        in->mi = mi;
        SendInput(1, in, sizeof(INPUT));
    }

    delete in; in = NULL;
}