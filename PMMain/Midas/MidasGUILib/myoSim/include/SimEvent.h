#pragma once
#include <cstdint>
#include "DeviceListenerSim.h"

#define ACCELEROMETER_MAX_INDEX 3
#define GYROSCOPE_MAX_INDEX 3

namespace myoSim
{
    enum class myoSimEvent {
        paired,
        unpaired,
        connected,
        disconnected,
        armRecognized,
        armLost,
        orientation,
        pose,
        rssi
    };

    enum class vectorIndex {
        first,
        second,
        third
    };

    class SimEvent
    {
    public:
        SimEvent();
        ~SimEvent();

        void setEventType(myoSimEvent type);
        void setTimestamp(uint64_t timestamp);
        void setMyoIdentifier(int identifier);
        void setArm(Arm arm);
        void setXDirection(XDirection xDirection);
        void setOrientation(float xOrientation, float yOrientation,
            float zOrientation, float wOrientation);
        void setAccelerometerData(float xAccel, float yAccel, float zAccel);
        void setGyroscopeData(float yawPerSecond, float pitchPerSecond, float rollPerSecond);
        void setPose(Pose pose);

        myoSimEvent getEventType() const;
        uint64_t getEventTimestamp() const;
        unsigned int getMyoIdentifier() const;
        Arm getArm() const;
        XDirection getXDirection() const;

        float getXOrientation() const;
        float getYOrientation() const;
        float getZOrientation() const;
        float getWOrientation() const;

        float getAccelerometerX() const;
        float getAccelerometerY() const;
        float getAccelerometerZ() const;

        float getGyroscopeYawPerSecond() const;
        float getGyroscopePitchPerSecond() const;
        float getGyroscopeRollPerSecond() const;

        Pose getPose() const;
        int8_t getRssi() const;

    private:
        myoSimEvent eventType;
        uint64_t eventTimestamp;
        unsigned int myoIdentifier;
        Arm arm;
        XDirection xDirection;
        float xOrientation, yOrientation, zOrientation, wOrientation;
        float accelX, accelY, accelZ;
        float gyroYaw, gyroPitch, gyroRoll;
        Pose pose;
        int8_t rssi;
    };

}