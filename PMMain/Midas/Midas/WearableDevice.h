#ifndef _WEARABLE_DEVICE_H
#define _WEARABLE_DEVICE_H

#include "SharedCommandData.h"

class WearableDevice;

void startWearableDeviceListener(WearableDevice* wearableDevice);

class WearableDevice {
public:
    WearableDevice(SharedCommandData* sharedCommandData) : sharedData(*sharedCommandData) { }
    virtual void runDeviceLoop() = 0;

protected:
    SharedCommandData& sharedData;
};

#endif /* _WEARABLE_DEVICE_H */