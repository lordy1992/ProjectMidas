#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"
#include "MouseCtrl.h"
#include "WearableDevice.h"
#include <vector>
#include <thread>
#include "SharedCommandData.h"

#include "SharedCommandDataTest.h"
#include "KybrdCtrlTest.h"
#include "MouseCtrlTest.h"

using namespace std;

#define MOUSE_CONTROL_TEST

int main() {

#ifdef TEST_WEARABLE_DEVICE
    SharedCommandDataTest::testQueue();
#endif

#ifdef KEYBOARD_CONTROL_TEST
    KybrdCtrlTest::testLock();
    KybrdCtrlTest::testAltTab();
#endif

#ifdef MOUSE_CONTROL_TEST
    cout << "Starting Move Mouse Square tests" << endl;
    MouseCtrlTest::testMouseSquare(300, 8); // slower
    MouseCtrlTest::testMouseSquare(300, 1); // fast
    

    cout << "Starting Scroll Mouse Square (up/down for now) tests" << endl;
    Sleep(1000);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, 4); //moderately slow 4/120 speed, and 5 ticks total.
    Sleep(500);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, 10); //moderately slow 10/120 speed, and 5 ticks total.
    Sleep(500);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, 40); // faster 40/120 speed, and 5 ticks total.
    Sleep(500);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, WHEEL_DELTA); // fastest 120/120 speed, and 5 ticks total.

    cout << "Starting Scroll Zoom Tests" << endl;
    Sleep(1000);
    MouseCtrlTest::testScrollZoomMouse(WHEEL_DELTA * 5, 4);
    Sleep(500);
    MouseCtrlTest::testScrollZoomMouse(WHEEL_DELTA * 5, 10);
    Sleep(500);
    MouseCtrlTest::testScrollZoomMouse(WHEEL_DELTA * 5, 20);

#endif

    system("PAUSE");
    return 0;
}