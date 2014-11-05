#include "KybrdCtrlTest.h"

void KybrdCtrlTest::testLock(void)
{
    // MANUAL TEST 1
    cout << "hijacking test main..." << endl;

    keybd_event(VK_LWIN, MapVirtualKey(VK_LWIN, MAPVK_VK_TO_VSC), 0, 0);
    Sleep(10);
    keybd_event(0x4C, MapVirtualKey(0x4C, MAPVK_VSC_TO_VK), 0, 0);
    Sleep(100);
    keybd_event(0x4C, MapVirtualKey(0x4C, MAPVK_VSC_TO_VK), KEYEVENTF_KEYUP, 0);
    Sleep(10);
    //keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, MapVirtualKey(VK_LWIN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
}

void KybrdCtrlTest::testAltTab(void)
{
    kybrdCtrl * controller = new kybrdCtrl();

    int count = 0;
    while (true) {
        if (count++ % 2 == 0) {
            //controller->setKeyCmd(kybdCmds::ZOOM_IN);
            //controller->setKeyChar('a');
            controller->setKeyCmd(kybdCmds::SWITCH_WIN_FORWARD);
        }
        else {
            //controller->setKeyCmd(kybdCmds::ZOOM_OUT);
            //controller->setKeyChar('b');
            //controller->setKeyCmd(kybdCmds::SWITCH_WIN_REVERSE);
        }

        int status = controller->sendData();
        cout << "looping with opposite commands... status = " << status << endl;

        Sleep(1000);
    }
}
