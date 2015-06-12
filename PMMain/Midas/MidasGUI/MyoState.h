#include <queue>
#include <mutex>
#include "myo\cxx\Vector3.hpp"
#include "myo\cxx\Quaternion.hpp"
#include "myo\cxx\Pose.hpp"

class MyoState {
public:
	void setSpatialHistLen(int spatialHistLen);
	void setPoseHistLen(int poseHistLen);

	void pushRotation(myo::Quaternion<float> rotation);
	myo::Quaternion<float> popRotation();

	void pushAccel(myo::Vector3<float> accel);
	myo::Vector3<float> popAccel();

	void pushGyro(myo::Vector3<float> gyro);
	myo::Vector3<float> popGryo();

	void pushPose(myo::Pose pose);
	myo::Pose popPose();
private:
	std::mutex myoStateMutex;

	int spatialHistLen;
	std::queue<myo::Quaternion<float>> rotationHistory;
	std::queue<myo::Vector3<float>> accelHistory;
	std::queue<myo::Vector3<float>> gyroHistory;

	int poseHistLen;
	std::queue<myo::Pose> poseHistory;
};