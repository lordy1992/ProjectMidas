#include "MyoDevice.h"


MyoDevice::MyoDevice(SharedCommandData* sharedCommandData, std::string applicationIdentifier)
    : WearableDevice(sharedCommandData), appIdentifier(applicationIdentifier), myoFindTimeout(DEFAULT_FIND_MYO_TIMEOUT),
        durationInMilliseconds(DEFAULT_MYO_DURATION_MS)
{
}

MyoDevice::~MyoDevice()
{
}

void MyoDevice::setFindMyoTimeout(unsigned int milliseconds)
{
    myoFindTimeout = milliseconds;
}

void MyoDevice::setMyoEventLoopDuration(unsigned int milliseconds)
{
    durationInMilliseconds = milliseconds;
}

void MyoDevice::runDeviceLoop()
{
    WearableDevice::setDeviceStatus(deviceStatus::RUNNING);

    try
    {
        Hub hub(appIdentifier);
        Myo* myo = hub.waitForMyo(myoFindTimeout);

        if (!myo)
        {
            WearableDevice::setDeviceStatus(deviceStatus::ERR);
            return;
        }

        MyoCallbacks myoCallbacks;
        hub.addListener(&myoCallbacks);

        while (true)
        {
            if (WearableDevice::stopDeviceRequested()) break;
            hub.run(durationInMilliseconds);
        }
    }
    catch (const std::exception& e)
    {
        WearableDevice::setDeviceStatus(deviceStatus::ERR);
        return;
    }

    WearableDevice::setDeviceStatus(deviceStatus::DONE);
}

int MyoDevice::getDeviceError()
{
    // TODO: Add error codes.
    return 0;
}

// Device Listener
MyoDevice::MyoCallbacks::~MyoCallbacks() { }

// Overridden functions from DeviceListener
// TODO: Implement
void MyoDevice::MyoCallbacks::onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) { }
void MyoDevice::MyoCallbacks::onUnpair(Myo* myo, uint64_t timestamp) { }
void MyoDevice::MyoCallbacks::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) { }
void MyoDevice::MyoCallbacks::onDisconnect(Myo* myo, uint64_t timestamp) { }
void MyoDevice::MyoCallbacks::onArmRecognized(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) { }
void MyoDevice::MyoCallbacks::onArmLost(Myo* myo, uint64_t timestamp) { }
void MyoDevice::MyoCallbacks::onPose(Myo* myo, uint64_t timestamp, Pose pose) { }
void MyoDevice::MyoCallbacks::onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation) { }
void MyoDevice::MyoCallbacks::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel) { }
void MyoDevice::MyoCallbacks::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro) { }
void MyoDevice::MyoCallbacks::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) { }