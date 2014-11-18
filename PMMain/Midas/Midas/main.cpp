#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"
#include "MouseCtrl.h"
#include "WearableDevice.h"
#include <vector>
#include <thread>
#include "SharedCommandData.h"
#include "MyoDevice.h"

#include "SharedCommandDataTest.h"
#include "KybrdCtrlTest.h"
#include "MouseCtrlTest.h"

using namespace std;

#define MYO_POSE_FILTER_TEST

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

    Sleep(3000);

    MouseCtrlTest::testMouseSquare(300, 8); // slower
    MouseCtrlTest::testMouseSquare(300, 4); // med
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

#ifdef MYO_POSE_FILTER_TEST
    SharedCommandData sharedData;
    ControlState controlState(&sharedData);
    MyoDevice* myoDevice = new MyoDevice(&sharedData, &controlState, "com.midas.midas-test");

    // Kick off device thread
    startWearableDeviceListener(myoDevice);

    while (true)
    {
        if (myoDevice->getDeviceStatus() != deviceStatus::RUNNING) break;

        commandData nextCmd;
        if (sharedData.consumeCommand(nextCmd))
        {
            if (nextCmd.mouse == LEFT_CLICK)
            {
                cout << "Received a left click." << endl;
            }
            else if (nextCmd.mouse == RIGHT_CLICK)
            {
                cout << "Received a right click." << endl;
            }
        }
    }

#endif

    system("PAUSE");
    return 0;
}