#include "MyoState.h"

void MyoState::setSpatialHistLen(int spatialHistLen)
{
	myoStateMutex.lock();
	this->spatialHistLen = spatialHistLen;
	myoStateMutex.unlock();
}

void MyoState::setPoseHistLen(int poseHistLen)
{
	myoStateMutex.lock();
	this->poseHistLen = poseHistLen;
	myoStateMutex.unlock();
}

void MyoState::pushRotation(myo::Quaternion<float> rotation)
{
	myoStateMutex.lock();
	rotationHistory.push(rotation);

	if (rotationHistory.size() > this->spatialHistLen)
	{
		rotationHistory.pop();
	}
	myoStateMutex.unlock();
}

myo::Quaternion<float> MyoState::popRotation()
{
	myoStateMutex.lock();
	myo::Quaternion<float> front = rotationHistory.front();

	myoStateMutex.unlock();

	return front;
}

void MyoState::pushAccel(myo::Vector3<float> accel)
{

}

myo::Vector3<float> MyoState::popAccel()
{
	return accelHistory.front();
}

void MyoState::pushGyro(myo::Vector3<float> gyro)
{

}

myo::Vector3<float> MyoState::popGryo()
{
	return gyroHistory.front();
}

void MyoState::pushPose(myo::Pose pose)
{

}

myo::Pose MyoState::popPose()
{
	return poseHistory.front();
}