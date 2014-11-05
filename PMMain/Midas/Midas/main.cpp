#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"
#include "WearableDevice.h"
#include "FilterPipeline.h"
#include <vector>
#include <thread>

using namespace std;

#define TEST_WEARABLE_DEVICE

#ifdef TEST_WEARABLE_DEVICE

class TestFilter : public Filter {
public:
    void process()
    {
        filterDataMap input = Filter::getInput();
        int cmd = boost::any_cast<int>(input["cmd"]);
        commandData command;

        if (cmd == 0)
        {
            command.type = KEYBOARD_COMMAND;
            command.kbd = UNDO;
        }
        else if (cmd == 1)
        {
            command.type = KEYBOARD_COMMAND;
            command.kbd = COPY;
        }
        else
        {
            command.type = MOUSE_COMMAND;
            command.mouse = LEFT_CLICK;
        }

        filterDataMap output;
        output[COMMAND_INPUT] = command;
        Filter::setOutput(output);
    }
};

class TestWearableClass : public WearableDevice 
{
public:
    TestWearableClass(SharedCommandData* sharedCommandData) : WearableDevice(sharedCommandData), done(false) 
    {
        commandFilter = new TestFilter();
        commandList.push_back(0);
        commandList.push_back(1);
        commandList.push_back(2);
    }

    ~TestWearableClass()
    {
        delete commandFilter;
    }

    void runDeviceLoop() 
    {
        FilterPipeline commandPipeline, coordPipeline;
        vector<int>::iterator it;

        commandPipeline.registerFilter(commandFilter);
        commandPipeline.registerFilter(WearableDevice::sharedData);

        coordPipeline.registerFilter(WearableDevice::sharedData);
        
        filterDataMap coordInput, commandInput;
        coordInput[COORD_INPUT] = point(100, 50);

        coordPipeline.startPipeline(coordInput);
        for (it = commandList.begin(); it != commandList.end(); it++) 
        {
            commandInput["cmd"] = *it;
            commandPipeline.startPipeline(commandInput);
            this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        
        coordInput[COORD_INPUT] = point(10, 10);
        coordPipeline.startPipeline(coordInput);

        done = true;
    }
    
    bool isDone() { return done; }
private:
    vector<int> commandList;
    bool done;
    Filter* commandFilter;
};

int main() 
{
    SharedCommandData sharedData;
    TestWearableClass* testDevice = new TestWearableClass(&sharedData);

    // Kick off device thread
    startWearableDeviceListener(testDevice);

    while (true) 
    {
        if (!sharedData.isCommandQueueEmpty()) 
        {
            commandData nextCmd;
            while (!sharedData.consumeCommand(nextCmd));
            point nextPnt = sharedData.getRelativeCoordinates();

            cout << "Command: " << (nextCmd.type == MOUSE_COMMAND ? nextCmd.mouse : nextCmd.kbd)
                << ", Coords: (" << nextPnt.x << ", " << nextPnt.y << ")" << endl;
        }
        else 
        {
            if (testDevice->isDone()) break;
        }
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
        keybd_event(VK_MENU, MapVirtualKey(VK_RMENU, MAPVK_VK_TO_VSC), 0, 0);
        Sleep(10);
        //keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC), 0, 0);
        //keybd_event(0x44, 0x20, 0, 0);
        //keybd_event(0x44, 0x20, KEYEVENTF_KEYUP, 0); 
        keybd_event(VK_TAB, MapVirtualKey(VK_TAB, MAPVK_VSC_TO_VK), 0, 0);
        Sleep(100);
        keybd_event(VK_TAB, MapVirtualKey(VK_TAB, MAPVK_VSC_TO_VK), KEYEVENTF_KEYUP, 0);
        Sleep(10);
        //keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, MapVirtualKey(VK_RMENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);

        Sleep(1000);
        cout << "still going..." << endl;
    } 


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
