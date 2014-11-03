#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"

using namespace std;

int main(void) {
    // MANUAL TEST 1
    cout << "hijacking test main..." << endl;

    kybrdCtrl * controller = new kybrdCtrl();

    //keybd_event(VK_LWIN, 0x5b, 0, 0);
        //keybd_event(0x44, 0x20, 0, 0);
        //keybd_event(0x44, 0x20, KEYEVENTF_KEYUP, 0); 
    //keybd_event(0x4C, MapVirtualKey(0x4C, MAPVK_VK_TO_VSC), 0, 0);
    //keybd_event(0x4C, MapVirtualKey(0x4C, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
    //keybd_event(VK_LWIN, 0x5b, KEYEVENTF_KEYUP, 0);

    //while (1) {}

    /*controller->setKeyCmd(kybdCmds::HIDE_APPS);
    int status = controller->sendData();
    cout << "status = " << status << endl;

    Sleep(2000);

    controller->setKeyCmd(kybdCmds::WIN_HOME);
    status = controller->sendData();
    cout << "status = " << status << endl; */


    string blah;
    cin >> blah;

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

    system("PAUSE");
    return 0;
}