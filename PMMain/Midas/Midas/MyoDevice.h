#pragma once
#include "WearableDevice.h"
#include "myo\myo.hpp"

#define DEFAULT_FIND_MYO_TIMEOUT 1000
#define DEFAULT_MYO_DURATION_MS 1000/20

using namespace myo;

class MyoDevice : public WearableDevice
{
public:
    MyoDevice(SharedCommandData* sharedCommandData, std::string applicationIdentifier);
    ~MyoDevice();

    void setFindMyoTimeout(unsigned int milliseconds);
    void setMyoEventLoopDuration(unsigned int milliseconds);

    // Overridden functions
    void runDeviceLoop();
    int getDeviceError();

private:
    class MyoCallbacks : public DeviceListener
    {
    public:
        ~MyoCallbacks();
        
        // Overridden functions from DeviceListener
        void onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion);
        void onUnpair(Myo* myo, uint64_t timestamp);
        void onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion);
        void onDisconnect(Myo* myo, uint64_t timestamp);
        void onArmRecognized(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection);
        void onArmLost(Myo* myo, uint64_t timestamp);
        void onPose(Myo* myo, uint64_t timestamp, Pose pose);
        void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation);
        void onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel);
        void onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro);
        void onRssi(Myo* myo, uint64_t timestamp, int8_t rssi);
    };

    unsigned int myoFindTimeout;
    unsigned int durationInMilliseconds;
    std::string appIdentifier;
};

