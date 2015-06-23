#ifndef MYO_STATE_H
#define MYO_STATE_H

#include <queue>
#include <mutex>
#include "myo\cxx\Vector3.hpp"
#include "myo\cxx\Quaternion.hpp"
#include "myo\cxx\Pose.hpp"

class MyoDevice;

class MyoState {
public:
	MyoState();

	void setSpatialHistLen(int spatialHistLen);
	void setPoseHistLen(int poseHistLen);

	void pushRotation(myo::Quaternion<float> rotation);
	myo::Quaternion<float> popRotation();
	std::queue<myo::Quaternion<float>> getRotationHistory();

	void pushAccel(myo::Vector3<float> accel);
	myo::Vector3<float> popAccel();
	std::queue<myo::Vector3<float>> getAccelHistory();

	void pushGyro(myo::Vector3<float> gyro);
	myo::Vector3<float> popGryo();
	std::queue<myo::Vector3<float>> getGyroHistory();

	void pushPose(myo::Pose pose);
	myo::Pose popPose();
	std::queue<myo::Pose> getPoseHistory();

    void setMyo(MyoDevice *myo);
    const MyoDevice* peakMyo();

private:
	std::mutex myoStateMutex;

	int spatialHistLen;
	std::queue<myo::Quaternion<float>> rotationHistory;
	std::queue<myo::Vector3<float>> accelHistory;
	std::queue<myo::Vector3<float>> gyroHistory;

	int poseHistLen;
	std::queue<myo::Pose> poseHistory;

    MyoDevice* myoHandle;
};

#endif /* MYO_STATE_H */