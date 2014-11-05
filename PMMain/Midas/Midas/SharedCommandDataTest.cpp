#include "SharedCommandDataTest.h"


void SharedCommandDataTest::testQueue(void)
{
    // Jeremy TODO - change name to reflect test better
    SharedCommandData sharedData;
    TestWearableClass* testDevice = new TestWearableClass(&sharedData);

    // Kick off device thread
    startWearableDeviceListener(testDevice);

    while (true) {
        if (sharedData.isCommandQueueEmpty() && testDevice->isDone()) break;

        commandData nextCmd;
        while (!sharedData.consumeCommand(nextCmd));
        point nextPnt = sharedData.getRelativeCoordinates();

        cout << "Command: " << (nextCmd.type == MOUSE_COMMAND ? nextCmd.mouse : nextCmd.kbd)
            << ", Coords: (" << nextPnt.x << ", " << nextPnt.y << ")" << endl;
    }
}