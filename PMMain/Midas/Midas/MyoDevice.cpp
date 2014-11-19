#include "MyoDevice.h"
#include "GestureFilter.h"
#include "MyoTranslationFilter.h"
#include "AveragingFilter.h"

MyoDevice::MyoDevice(SharedCommandData* sharedCommandData, ControlState* controlState, std::string applicationIdentifier)
    : WearableDevice(sharedCommandData), appIdentifier(applicationIdentifier), myoFindTimeout(DEFAULT_FIND_MYO_TIMEOUT),
        durationInMilliseconds(DEFAULT_MYO_DURATION_MS), state(controlState)
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

    GestureFilter gestureFilter(state, 0);
    posePipeline.registerFilter(&gestureFilter);
    posePipeline.registerFilter(WearableDevice::sharedData);

    AveragingFilter averagingFilter(5);
    MyoTranslationFilter translationFilter(state);
    orientationPipeline.registerFilter(&averagingFilter);
    orientationPipeline.registerFilter(&translationFilter);
    orientationPipeline.registerFilter(WearableDevice::sharedData);

    try
    {
        Hub hub(appIdentifier);
        Myo* myo = hub.waitForMyo(myoFindTimeout);

        if (!myo)
        {
            std::cout << "Could not find a Myo." << std::endl;
            WearableDevice::setDeviceStatus(deviceStatus::ERR);
            return;
        }

        MyoCallbacks myoCallbacks(*this);
        hub.addListener(&myoCallbacks);

        while (true)
        {
            if (WearableDevice::stopDeviceRequested()) 
            {
                break;
            }
            hub.run(durationInMilliseconds);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
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
MyoDevice::MyoCallbacks::MyoCallbacks(MyoDevice& parentDevice) 
    : parent(parentDevice)
{
}

MyoDevice::MyoCallbacks::~MyoCallbacks() { }

// Overridden functions from DeviceListener
void MyoDevice::MyoCallbacks::onPose(Myo* myo, uint64_t timestamp, Pose pose) 
{
    filterDataMap input;
    input[GESTURE_INPUT] = pose.type();

    parent.posePipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation) 
{ 
    filterDataMap input;
    input[QUAT_DATA_X] = rotation.x();
    input[QUAT_DATA_Y] = rotation.y();
    input[QUAT_DATA_Z] = rotation.z();
    input[QUAT_DATA_W] = rotation.w();

    // The following is junk data. The averaging filter should be modified so
    // that it doesn't deal with the data so specifically.
    input[ACCEL_DATA_X] = 0.0f;
    input[ACCEL_DATA_Y] = 0.0f;
    input[ACCEL_DATA_Z] = 0.0f;
    input[GYRO_DATA_X] = 0.0f;
    input[GYRO_DATA_Y] = 0.0f;
    input[GYRO_DATA_Z] = 0.0f;
    
    parent.orientationPipeline.startPipeline(input);
}

// TODO: Implement
void MyoDevice::MyoCallbacks::onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) { }
void MyoDevice::MyoCallbacks::onUnpair(Myo* myo, uint64_t timestamp) { }
void MyoDevice::MyoCallbacks::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) { }
void MyoDevice::MyoCallbacks::onDisconnect(Myo* myo, uint64_t timestamp) { }
void MyoDevice::MyoCallbacks::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) { }
void MyoDevice::MyoCallbacks::onArmUnsync(Myo* myo, uint64_t timestamp) { }
void MyoDevice::MyoCallbacks::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel) { }
void MyoDevice::MyoCallbacks::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro) { }
void MyoDevice::MyoCallbacks::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) { }