#ifndef _WEARABLE_DEVICE_H
#define _WEARABLE_DEVICE_H

#include "SharedCommandData.h"

class WearableDevice;

void startWearableDeviceListener(WearableDevice* wearableDevice);

enum class deviceStatus
{
    RUNNING,
    DONE,
    ERR
};

class WearableDevice 
{
public:
    WearableDevice(SharedCommandData* sharedCommandData) : sharedData(sharedCommandData), stopFlag(false) { }
    virtual void runDeviceLoop() = 0;

    // Not necessarily thread-safe. This should only be called once
    // the device thread has stopped. The error code is device-specific.
    virtual int getDeviceError() = 0;

    // Thread-safe
    deviceStatus getDeviceStatus();
    void stopDevice();

protected:
    void setDeviceStatus(deviceStatus newStatus);

    // It is the device's responsibility to check this method to 
    // determine if the main thread has requested it to stop. If it has
    // been sent a stop request, the device must perform cleanup and exit its
    // main loop.
    bool stopDeviceRequested();

    SharedCommandData* sharedData;

private:
    std::mutex statusLock;
    std::mutex stopFlagMutex;
    deviceStatus status;
    bool stopFlag;
};

#endif /* _WEARABLE_DEVICE_H */