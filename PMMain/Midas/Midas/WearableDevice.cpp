#include "WearableDevice.h"
#include <thread>

static void wearableThreadWrapper(WearableDevice* wearableDevice);

deviceStatus WearableDevice::getDeviceStatus()
{
    deviceStatus statusReturn;
    statusLock.lock();
    statusReturn = status;
    statusLock.unlock();
    return statusReturn;
}

void WearableDevice::setDeviceStatus(deviceStatus newStatus)
{
    statusLock.lock();
    status = newStatus;
    statusLock.unlock();
}

void WearableDevice::stopDevice()
{
    stopFlagMutex.lock();
    stopFlag = true;
    stopFlagMutex.unlock();
}

bool WearableDevice::stopDeviceRequested()
{
    bool flagResult;
    stopFlagMutex.lock();
    flagResult = stopFlag;
    stopFlagMutex.unlock();
    return flagResult;
}

/*
 * This kicks off the thread that runs the wearable device's main loop. This loop is
 * used to collect information from the device and pass actions (commands, coordinated of the mouse)
 * to the shared data (which wearableDevice has a reference to).
 */
void startWearableDeviceListener(WearableDevice* wearableDevice)
{
    std::thread devThread(wearableThreadWrapper, wearableDevice);
    devThread.detach();
}

void wearableThreadWrapper(WearableDevice* wearableDevice)
{
    wearableDevice->runDeviceLoop();

    do {
        std::this_thread::yield();
    } while (true);
}

