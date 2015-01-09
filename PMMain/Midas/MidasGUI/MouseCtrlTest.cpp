#include "MouseCtrlTest.h"
#include "kybrdCtrl.h"
#include <time.h>

void MouseCtrlTest::testMouseSquare(DWORD mouseRate)
{
    cout << "Welcome to the Mouse Ctrl Test. The mouse will now go in a square autonomously." << endl;
    
    MouseCtrl *mc = new MouseCtrl();

    // all distance should be equivelant to 100 ms at full speed.
    DWORD loopTime = (100.0 / mouseRate) * 200; 

    DWORD t1 = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD t2 = clock() * (1000 / CLOCKS_PER_SEC);
    while (t2 - t1 < loopTime)
    {
        mc->sendCommand(mouseCmds::MOVE_UP, true, mouseRate);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    t1 = t2;
    while (t2 - t1 < loopTime)
    {
        mc->sendCommand(mouseCmds::MOVE_RIGHT, true, mouseRate);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    t1 = t2;
    while (t2 - t1 < loopTime)
    {
        mc->sendCommand(mouseCmds::MOVE_DOWN, true, mouseRate);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    t1 = t2;
    while (t2 - t1 < loopTime)
    {
        mc->sendCommand(mouseCmds::MOVE_LEFT, true, mouseRate);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }

    delete mc; mc = NULL;
}

void MouseCtrlTest::testScrollMouseSquare(unsigned int loopSize, int scrollRate)
{
    loopSize /= scrollRate; // This adjusts it so that the expected loopSize states how many scroll 'clicks' ACTUALLY should happen.
    cout << "Welcome to the Mouse Scroll Test. The mouse will scroll autonomously up/down. Sides not working." << endl;

    MouseCtrl *mc = new MouseCtrl();
    mc->setScrollRate(scrollRate);

    DWORD t1 = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD t2 = clock() * (1000 / CLOCKS_PER_SEC);
    while (t2 - t1 < SCROLL_MIN_TIME*loopSize)
    {
        mc->sendCommand(mouseCmds::SCROLL_UP);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    t1 = t2;
    /*while (t2 - t1 < SCROLL_MIN_TIME*loopSize)
    {
        mc->sendCommand(mouseCmds::SCROLL_RIGHT);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    t1 = t2;*/ // not working
    while (t2 - t1 < SCROLL_MIN_TIME*loopSize)
    {
        mc->sendCommand(mouseCmds::SCROLL_DOWN);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    /*t1 = t2;
    while (t2 - t1 < SCROLL_MIN_TIME*loopSize)
    {
        mc->sendCommand(mouseCmds::SCROLL_LEFT);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    } */ // not working

    /*
    for (int i = 0; i < loopSize; i++)
    {
        mc->sendCommand(mouseCmds::SCROLL_UP);
        Sleep(500);
    }
    for (int i = 0; i < loopSize; i++)
    {
        mc->sendCommand(mouseCmds::SCROLL_RIGHT);
        Sleep(500);
    }
    for (int i = 0; i < loopSize; i++)
    {
        mc->sendCommand(mouseCmds::SCROLL_DOWN);
        Sleep(500);
    }
    for (int i = 0; i < loopSize; i++)
    {
        mc->sendCommand(mouseCmds::SCROLL_LEFT);
        Sleep(500);
    }*/

    delete mc; mc = NULL;
}

void MouseCtrlTest::testScrollZoomMouse(unsigned int size, int scrollRate)
{
    size /= scrollRate; // This adjusts it so that the expected loopSize states how many scroll 'clicks' ACTUALLY should happen.
    cout << "Welcome to the Mouse Scroll Test. The mouse will scroll autonomously up/down. Sides not working." << endl;

    KybrdCtrl *kc = new KybrdCtrl();
    kc->setKeyCmd(kybdCmds::CONTROL, false); // push and hold control.
    kc->sendData();

    MouseCtrl *mc = new MouseCtrl();
    mc->setScrollRate(scrollRate);

    DWORD t1 = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD t2 = clock() * (1000 / CLOCKS_PER_SEC);
    while (t2 - t1 < SCROLL_MIN_TIME*size)
    {
        mc->sendCommand(mouseCmds::SCROLL_UP);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }
    t1 = t2;
    while (t2 - t1 < SCROLL_MIN_TIME*size)
    {
        mc->sendCommand(mouseCmds::SCROLL_DOWN);
        t2 = clock() * (1000 / CLOCKS_PER_SEC);
    }

    kc->setKeyCmd(kybdCmds::CONTROL); // release control.
    kc->sendData();

    delete mc; mc = NULL;
    delete kc; kc = NULL;
}