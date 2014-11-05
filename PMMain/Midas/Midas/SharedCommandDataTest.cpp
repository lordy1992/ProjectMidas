#include "SharedCommandDataTest.h"

void SharedCommandDataTest::testQueue(void)
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
}