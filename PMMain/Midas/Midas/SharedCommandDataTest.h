#ifndef SHARED_COMMAND_DATA_TEST
#define SHARED_COMMAND_DATA_TEST

#include <iostream>
#include <string>
#include "WearableDevice.h"
#include <vector>
#include <thread>
using namespace std;

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

class SharedCommandDataTest
{
public:
    void testQueue(void);
};
#endif /* SHARED_COMMAND_DATA_TEST */