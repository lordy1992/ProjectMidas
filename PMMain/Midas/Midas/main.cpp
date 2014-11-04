#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"
#include "WearableDevice.h"
#include <vector>
#include <thread>

using namespace std;

#define KEYBOARD_CONTROL_TEST

#ifdef TEST_WEARABLE_DEVICE

class TestWearableClass : public WearableDevice {
public:
    TestWearableClass(SharedCommandData* sharedCommandData) : WearableDevice(sharedCommandData), done(false) {
        commandData command;
        command.type = KEYBOARD_COMMAND;
        command.kbd = UNDO;
        commandList.push_back(command);

        command.type = KEYBOARD_COMMAND;
        command.kbd = COPY;
        commandList.push_back(command);

        command.type = MOUSE_COMMAND;
        command.mouse = LEFT_CLICK;
        commandList.push_back(command);
    }

    void runDeviceLoop() {
        vector<commandData>::iterator it;

        WearableDevice::sharedData.setRelativeCoordinates(point(100, 50));
        for (it = commandList.begin(); it != commandList.end(); it++) {
            WearableDevice::sharedData.addCommand(*it);
           // this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        
        WearableDevice::sharedData.setRelativeCoordinates(point(10, 10));

        done = true;
    }
    
    bool isDone() { return done; }
private:
    vector<commandData> commandList;
    bool done;
};

int main() {
    SharedCommandData sharedData;
    TestWearableClass* testDevice = new TestWearableClass(&sharedData);

    // Kick off device thread
    startWearableDeviceListener(testDevice);

    while (true) {
        if (sharedData.isCommandQueueEmpty() && testDevice->isDone()) break;

        commandData nextCmd;
        while(!sharedData.consumeCommand(nextCmd));
        point nextPnt = sharedData.getRelativeCoordinates();

        cout << "Command: " << (nextCmd.type == MOUSE_COMMAND ? nextCmd.mouse : nextCmd.kbd)
            << ", Coords: (" << nextPnt.x << ", " << nextPnt.y << ")" << endl;
    }

    system("PAUSE");
    return 0;
}

#endif 

#ifdef KEYBOARD_CONTROL_TEST
int main(void) {
    // MANUAL TEST 1
    cout << "hijacking test main..." << endl;

    kybrdCtrl * controller = new kybrdCtrl();

    while (1) {
        keybd_event(VK_MENU, 0, 0, 0);// MapVirtualKey(VK_RMENU, MAPVK_VK_TO_VSC), 0, 0);
        Sleep(10);
        //keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC), 0, 0);
        //keybd_event(0x44, 0x20, 0, 0);
        //keybd_event(0x44, 0x20, KEYEVENTF_KEYUP, 0); 
        keybd_event(VK_TAB, 0, 0, 0);// MapVirtualKey(0x4C, VK_TAB), 0, 0);
        Sleep(100);
        keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);// MapVirtualKey(0x4C, VK_TAB), KEYEVENTF_KEYUP, 0);
        Sleep(10);
        //keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);// MapVirtualKey(VK_RMENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);

        Sleep(1000);
    }

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
#endif
