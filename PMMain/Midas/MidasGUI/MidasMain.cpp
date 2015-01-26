#include "MidasMain.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include "SCDDigester.h"
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

int midasMain(MidasThread *threadHandle, SequenceDisplayer* sequenceDisplayer, InfoIndicator* infoIndicator) {
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
    SharedCommandData sharedData;
    ControlState controlState(&sharedData);
    MyoDevice* myoDevice = new MyoDevice(&sharedData, &controlState, "com.midas.midas-test", sequenceDisplayer, infoIndicator);
    MouseCtrl* mouseCtrl = new MouseCtrl();
    KybrdCtrl* kybrdCtrl = new KybrdCtrl();

    // Kick off device thread
    startWearableDeviceListener(myoDevice);

    SCDDigester scdDigester(&sharedData, threadHandle, &controlState, mouseCtrl, kybrdCtrl);
    
    while (true)
    {
        if (myoDevice->getDeviceStatus() != deviceStatus::RUNNING) break;

        scdDigester.digest();
    }

#endif

    system("PAUSE");
    return 0;
}