#pragma once
#include "WearableDevice.h"
#include "FilterPipeline.h"
#include "ControlState.h"
#include "myo\myo.hpp"
#include "MainGUI.h"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

#define DEFAULT_FIND_MYO_TIMEOUT 1000
#define DEFAULT_MYO_DURATION_MS 1000/20
#define DEFAULT_MYO_ARM Arm::armUnknown
#define DEFAULT_MYO_XDIR XDirection::xDirectionUnknown


/**
 * Handles the Myo device, collecting the data using the Myo API, and converting the data
 * to keyboard and mouse commands by sending it through a data pipeline.
 */
class MyoDevice : public WearableDevice
{
public:
    /**
     * The constructor for the MyoDevice.
     *
     * @param sharedCommandData A handle to the SCD so that the device can post commands.
     * @param controlState A handle to the ControlState object so that the application state can be 
     * changed, and so that the device can read the current state.
     * @param applicationIdentifier A myo-specific app identifier used to create the myo hub.
     */
    MyoDevice(SharedCommandData* sharedCommandData, ControlState* controlState, std::string applicationIdentifier, 
        MainGUI *mainGuiHandle);
    ~MyoDevice();

    /**
     * Sets the amount of time that should be spent looking for a Myo device. If a Myo
     * cannot be found within this amount of time, the device thread will exit with an
     * error.
     *
     * @param milliseconds Number of milliseconds to wait for a Myo. If this is a 0, it will
     * search until one is found (indefinitely).
     */
    void setFindMyoTimeout(unsigned int milliseconds);

    /**
     * Sets the amount of time, in milliseconds, the Myo event loop should run. While the Myo
     * event loop is running, Midas has no control over the device. The device can only be exited while
     * outside of the Myo event loop.
     *
     * @param milliseconds Number of milliseconds to stay in the event loop.
     */
    void setMyoEventLoopDuration(unsigned int milliseconds);

    // Overridden functions
    /**
     * Runs the device loop. This sets up the data pipeline, searches for a Myo device, registers
     * the callback functions, and then enters a loop that waits for events and calls the callback
     * functions.
     */
    void runDeviceLoop();

    /**
     * Returns an error code associated with the device, once the device thread has stopped running.
     *
     * @return An error code.
     */
    int getDeviceError();

private:
    /**
     * This class implements all of the callback functions from the Myo DeviceListener
     * class. The methods in this class are called when Myo events occur.
     */
    class MyoCallbacks : public DeviceListener
    {
    public:
        MyoCallbacks(MyoDevice& parentDevice);
        ~MyoCallbacks();
        
        // Overridden functions from DeviceListener
        void onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion);
        void onUnpair(Myo* myo, uint64_t timestamp);
        void onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion);
        void onDisconnect(Myo* myo, uint64_t timestamp);
        void onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection);
        void onArmUnsync(Myo* myo, uint64_t timestamp);
        // For SDK <= 5, use these 2 arm callbacks.
        void onArmRecognized(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) { onArmSync(myo, timestamp, arm, xDirection); }
        void onArmLost(Myo* myo, uint64_t timestamp) { onArmUnsync(myo, timestamp); }
        void onPose(Myo* myo, uint64_t timestamp, Pose pose);
        void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation);
        void onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel);
        void onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro);
        void onRssi(Myo* myo, uint64_t timestamp, int8_t rssi);

    private:
        MyoDevice& parent;
    };

    unsigned int myoFindTimeout;
    unsigned int durationInMilliseconds;
    std::string appIdentifier;
    ControlState* state;
    FilterPipeline posePipeline, orientationPipeline, rssiPipeline;
    MainGUI *mainGui;

    Arm arm;
    XDirection xDirection;
};

