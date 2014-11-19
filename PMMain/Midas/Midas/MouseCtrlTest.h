#ifndef _MOUSE_CTRL_TEST
#define _MOUSE_CTRL_TEST

#include <iostream>
#include <string>
#include <Windows.h>
#include "MouseCtrl.h"
using namespace std;

class MouseCtrlTest
{
public:
    static void testMouseSquare(int loopSize, DWORD minDelta);

    static void testScrollMouseSquare(int loopSize, int scrollRate);

    static void testScrollZoomMouse(int size, int scrollRate);
};

#endif /* _MOUSE_CTRL_TEST */