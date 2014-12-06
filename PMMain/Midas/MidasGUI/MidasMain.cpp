#include "MidasMain.h"

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

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#endif

#ifdef USE_SIMULATOR
using namespace myoSim;
#else
using namespace myo;
#endif

// TODO!!! TEMP only -- obtaining global handle on FIRST created MidasThread (ideally the only one) and using that to emitString.
#include "GestureSeqRecorder.h"

int midasMain(MidasThread *threadHandle) {
    std::cout << "starting Midas Main" << std::endl;

    threadHandle->emitInfo();
    Sleep(1000);
    threadHandle->threadEmitString("testString1");
    gMidasThread = threadHandle;
    gThreadHandle = threadHandle;
    Sleep(1000);
    threadHandle->threadEmitString("testString2");

#ifdef TEST_WEARABLE_DEVICE
    SharedCommandDataTest::testQueue();
#endif

#ifdef KEYBOARD_CONTROL_TEST
    KybrdCtrlTest::testLock();
    KybrdCtrlTest::testZoomInOut();
#endif

#ifdef MOUSE_CONTROL_TEST
    cout << "Starting Move Mouse Square tests" << endl;

    Sleep(3000);

    MouseCtrlTest::testMouseSquare(25); // slower
    MouseCtrlTest::testMouseSquare(50); // med
    MouseCtrlTest::testMouseSquare(100); // fast
    
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
    MouseCtrl* mouseCtrl = new MouseCtrl();

    // Kick off device thread
    startWearableDeviceListener(myoDevice);

    time_t beginTime = clock();
    commandData prevCmd;
    int count = 0;
    while (true)
    {
        if (myoDevice->getDeviceStatus() != deviceStatus::RUNNING) break;

        commandData nextCmd;
        if (sharedData.consumeCommand(nextCmd))
        {
            if (nextCmd.mouse == LEFT_CLICK)
            {
                cout << "Received a left click." << endl;
                mouseCtrl->sendCommand(mouseCmds::LEFT_CLICK, false);
            }
            else if (nextCmd.mouse == RIGHT_CLICK)
            {
                cout << "Received a right click." << endl;
                mouseCtrl->sendCommand(mouseCmds::RIGHT_CLICK, false);
            }
            else if (nextCmd.mouse == LEFT_RELEASE)
            {
                cout << "Received a left release." << endl;
                mouseCtrl->sendCommand(mouseCmds::LEFT_CLICK, true);
            }
            else if (nextCmd.mouse == RIGHT_RELEASE)
            {
                cout << "Received a right release." << endl;
                mouseCtrl->sendCommand(mouseCmds::RIGHT_CLICK, true);
            }
        }

        point unitVelocity = sharedData.getVelocity();
        if (unitVelocity.x != 0)
        {
            mouseCtrl->sendCommand(mouseCmds::MOVE_HOR, true, unitVelocity.x);
        }
        if (unitVelocity.y != 0)
        {
            mouseCtrl->sendCommand(mouseCmds::MOVE_VERT, true, unitVelocity.y);
        }

        if (clock() - beginTime >= 1000)
        {
            cout << "Percent of X: " << unitVelocity.x << ", Percent of Y: " << unitVelocity.y << endl;
            beginTime = clock();
        }
        prevCmd = nextCmd;

        if (count % 100000 == 0)
        {
            threadHandle->threadEmitString(std::to_string(count)); // this proves we can modify gui from here! woot.
            gMidasThread = threadHandle; // this isnt working, but i htink that's due to thread boundaries. so this needs to be properly coded. TODO
        }
        count++;
    }

#endif

    system("PAUSE");
    return 0;
}