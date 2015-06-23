#include "MouseCtrl.h"
#include "BaseMeasurements.h"
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
    currHeld = 0;
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
    minMoveXTimeDelta = convertRateToDelta(rate);
}

void MouseCtrl::setMinMoveYTimeDelta(unsigned int rate)
{
    
    minMoveYTimeDelta = convertRateToDelta(rate);
}

unsigned int MouseCtrl::convertRateToDelta(unsigned int rate)
{
    if (rate < MOVE_RATE_DEADZONE)
    {
        return MAXUINT;
    }
    else if (rate < MOVE_RATE_DEADZONE + MOVE_RATE_SLOWZONE)
    {
        return MAX_MOVE_TIME_DELTA;
    }

    float ratePercent = min(1.0, max(((rate - MOVE_RATE_DEADZONE - MOVE_RATE_SLOWZONE) / (100.0 - MOVE_RATE_DEADZONE - MOVE_RATE_SLOWZONE)), 0));

    // calc min/max velocity in pixels/ms
    float maxVeloc = NUM_PIXEL_MOVE / (float)MIN_MOVE_TIME_DELTA;
    float minVeloc = NUM_PIXEL_MOVE / (float)MAX_MOVE_TIME_DELTA;
    float deltaVeloc = maxVeloc - minVeloc;

    float currVeloc = (deltaVeloc * ratePercent) + minVeloc;
    if (currVeloc <= 0.0)
    {
        // should not happen
        return MAXUINT;
    }

    return min(max(ceil(NUM_PIXEL_MOVE / currVeloc), MIN_MOVE_TIME_DELTA), MAX_MOVE_TIME_DELTA);
}

void MouseCtrl::sendCommand(mouseCmds mouseCmd, double mouseRateIfMove, double mouseRateIfMoveY_hack)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    DWORD currentTime = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD deltaTimeXMove = currentTime - lastMouseMoveX;
    DWORD deltaTimeYMove = currentTime - lastMouseMoveY;
    DWORD deltaTimeScroll = currentTime - lastMouseScroll;

	setMouseInputVars(mouseCmd, mouseRateIfMove, mouseRateIfMoveY_hack);

    // Handle Early exit cases if moving mouse
    if (mi.dwFlags == MOUSEEVENTF_MOVE 
        && 
        (((deltaTimeXMove < minMoveXTimeDelta && mi.dx != 0) ||
        (deltaTimeYMove < minMoveYTimeDelta && mi.dy != 0))
        || 
        (mouseRateIfMove > 0 && mouseRateIfMove < MOVE_RATE_DEADZONE)
        ||
        (mi.dx != 0 && mi.dy != 0 && mouseCmd != mouseCmds::MOVE_ABSOLUTE))
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

    // Build and send opposite command if clicking!
    if (mouseCmd == mouseCmds::LEFT_CLICK ||
        mouseCmd == mouseCmds::RIGHT_CLICK ||
        mouseCmd == mouseCmds::MIDDLE_CLICK)
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

        Sleep(10);
        in->type = INPUT_MOUSE;
        in->mi = mi;
        SendInput(1, in, sizeof(INPUT));
    }

    done:
    delete in; in = NULL;
}

void MouseCtrl::setMouseInputVars(mouseCmds mouseCmd, double& mouseRateIfMove, double& mouseRateIfMoveY_hack)
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

	if (mouseCmd != mouseCmds::MOVE_ABSOLUTE)
	{
		mouseRateIfMove = abs(mouseRateIfMove);
	}

    switch (mouseCmd)
    {
    case mouseCmds::LEFT_HOLD:
        currHeld |= LEFT_HELD;
    case mouseCmds::LEFT_CLICK:
        mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case mouseCmds::RIGHT_HOLD:
        currHeld |= RIGHT_HELD;
    case mouseCmds::RIGHT_CLICK:
        mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case mouseCmds::MIDDLE_HOLD:
        currHeld |= MID_HELD;
    case mouseCmds::MIDDLE_CLICK:
        mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    case mouseCmds::LEFT_RELEASE:
        currHeld &= !LEFT_HELD;
        mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case mouseCmds::RIGHT_RELEASE:
        currHeld &= !RIGHT_HELD;
        mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    case mouseCmds::MIDDLE_RELEASE:
        currHeld &= !MID_HELD;
        mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    case mouseCmds::RELEASE_LRM_BUTS:
        mi.dwFlags = 0;
        if ((currHeld & LEFT_HELD) != 0)
        {
            currHeld &= !LEFT_HELD;
            mi.dwFlags |= MOUSEEVENTF_LEFTUP;
        }
        if ((currHeld & RIGHT_HELD) != 0)
        {
            currHeld &= !RIGHT_HELD;
            mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
        }
        if ((currHeld & MID_HELD) != 0)
        {
            currHeld &= !MID_HELD;
            mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
        }
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
	case mouseCmds::MOVE_ABSOLUTE:
		if (!BaseMeasurements::getInstance().areCurrentValuesValid())
		{
			break;
		}

		mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		int monitorSizeWeight = 65535; // size of a single monitor as represented by windows API
		float monitorWidth = 1920.0; // TEMP TODO - make variable perhaps? For now, this is size of expected monitors
		float monitorHeight = 1080.0;

		float baseCursorX = BaseMeasurements::getInstance().getBaseCursorX();
        float baseCursorY = BaseMeasurements::getInstance().getBaseCursorY();
        float baseWindowsLocX = (baseCursorX / monitorWidth) * monitorSizeWeight;
        float baseWindowsLocY = (baseCursorY / monitorHeight) * monitorSizeWeight;

		//mi.dy = monitorSizeWeight / 2 + (mouseRateIfMoveY_hack / 100.0 * monitorSizeWeight / 2);
		//mi.dx = monitorSizeWeight / 2 + (mouseRateIfMove / 100.0 * monitorSizeWeight / 2);
		mi.dx = baseWindowsLocX + (mouseRateIfMove / 100.0 * monitorSizeWeight / 2);
		mi.dy = baseWindowsLocY + (mouseRateIfMoveY_hack / 100.0 * monitorSizeWeight / 2);

		mi.dx = max(min(mi.dx, monitorSizeWeight), 0);
		mi.dy = max(min(mi.dy, monitorSizeWeight), 0);

		if (mi.dx < 10 && mi.dy < 10)
		{
			int a = 1;
		}

		break;
    }
}