#include "MidasMain.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include "SCDDigester.h"
#include "kybrdCtrl.h"
#include "MouseCtrl.h"
#include "ringData.h"
#include "WearableDevice.h"
#include <vector>
#include <thread>
#include "SharedCommandData.h"
#include "MyoDevice.h"

#include "MidasThread.h"
#include "SharedCommandDataTest.h"
#include "KybrdCtrlTest.h"
#include "MouseCtrlTest.h"
#include "KeyboardSettingsReader.h"

using namespace std;

#define MAIN_MODE

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#endif

#ifdef USE_SIMULATOR
using namespace myoSim;
#else
using namespace myo;
#endif

int midasMain(MidasThread *threadHandle, MainGUI *mainGui) {
    std::cout << "starting Midas Main" << std::endl;

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
#endif

#ifdef MAIN_MODE
    SharedCommandData sharedData;
    ControlState controlState(&sharedData);
    MyoDevice* myoDevice = new MyoDevice(&sharedData, &controlState, "com.midas.midas-test", mainGui);
    MouseCtrl* mouseCtrl = new MouseCtrl();
    KybrdCtrl* kybrdCtrl = new KybrdCtrl();
    vector<ringData> kybrdRingData;
    KeyboardSettingsReader readFile;
    readFile.readKeyboardSetupFile(kybrdRingData);

    // Kick off device thread
    startWearableDeviceListener(myoDevice); // TODO - add a flag in myoDevice to see if it is running. Don't enter 'while true' until running.

    SCDDigester scdDigester(&sharedData, threadHandle, &controlState, mouseCtrl, kybrdCtrl, kybrdRingData);
    
    while (true)
    {
        if (myoDevice->getDeviceStatus() != deviceStatus::RUNNING) break;

        scdDigester.digest();
    }

#endif

    exit(0);
    return 0;
}