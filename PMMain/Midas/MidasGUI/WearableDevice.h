#ifndef _WEARABLE_DEVICE_H
#define _WEARABLE_DEVICE_H

#include "SharedCommandData.h"

class WearableDevice;

/**
 * Kicks off the wearable device thread.
 *
 * @param wearableDevice The device to start running.
 */
void startWearableDeviceListener(WearableDevice* wearableDevice);

/**
 * The status of the device, that can be returned after the device has finished
 * running.
 */
enum class deviceStatus
{
    RUNNING,    /**< The device is still running. */
    DONE,       /**< The device has finished without error. */
    ERR         /**< The device has finished with an error. */
};

/**
 * Handles a specific Midas device. 
 */
class WearableDevice 
{
public:
    WearableDevice(SharedCommandData* sharedCommandData) : sharedData(sharedCommandData), stopFlag(false) { }

    /**
     * Runs the device loop, handling specific event handling for the device.
     */
    virtual void runDeviceLoop() = 0;

    /**
     * This returns a device-specific error code from the device, once it has
     * stopped running. This is not necessarily thread-safe and should only be 
     * called once the device thread has stopped.
     *
     * @return The device-specific error code.
     */
    virtual int getDeviceError() = 0;

    /**
     * Returns the status of the device. This is thread-safe.
     *
     * @return The status of the device.
     */
    deviceStatus getDeviceStatus();

    /**
     * Stops the device. This gives the device a chance to respond 
     * and perform cleanup before exiting.
     */
    void stopDevice();

protected:
    /**
     * Sets the status of the device.
     *
     * @param newStatus The new status of the device.
     */
    void setDeviceStatus(deviceStatus newStatus);

    /**
     * Indicates if the device has been requested to stop (by the main thread). It is
     * the device's responsibility to check this method and, if requested to stop, perform
     * the necessary cleanup and exit the main loop.
     */
    bool stopDeviceRequested();

    SharedCommandData* sharedData;

private:
    std::mutex statusLock;
    std::mutex stopFlagMutex;
    deviceStatus status;
    bool stopFlag;
};

#endif /* _WEARABLE_DEVICE_H */