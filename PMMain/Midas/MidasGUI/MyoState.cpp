#include "MyoState.h"
#include "MyoDevice.h"

MyoState::MyoState()
{
	spatialHistLen = 1;
	poseHistLen = 1;
}

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

std::queue<myo::Quaternion<float>> MyoState::getRotationHistory()
{
	myoStateMutex.lock();
	std::queue<myo::Quaternion<float>> copy = std::queue<myo::Quaternion<float>>(rotationHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::pushAccel(myo::Vector3<float> accel)
{
	myoStateMutex.lock();
	accelHistory.push(accel);

	if (accelHistory.size() > this->spatialHistLen)
	{
		accelHistory.pop();
	}
	myoStateMutex.unlock();
}

myo::Vector3<float> MyoState::popAccel()
{
	myoStateMutex.lock();
	myo::Vector3<float> front = accelHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::queue<myo::Vector3<float>> MyoState::getAccelHistory()
{
	myoStateMutex.lock();
	std::queue<myo::Vector3<float>> copy = std::queue<myo::Vector3<float>>(accelHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::pushGyro(myo::Vector3<float> gyro)
{
	myoStateMutex.lock();
	gyroHistory.push(gyro);

	if (gyroHistory.size() > this->spatialHistLen)
	{
		gyroHistory.pop();
	}
	myoStateMutex.unlock();
}

myo::Vector3<float> MyoState::popGryo()
{
	myoStateMutex.lock();
	myo::Vector3<float> front = gyroHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::queue<myo::Vector3<float>> MyoState::getGyroHistory()
{
	myoStateMutex.lock();
	std::queue<myo::Vector3<float>> copy = std::queue<myo::Vector3<float>>(gyroHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::pushPose(myo::Pose pose)
{
	myoStateMutex.lock();
	poseHistory.push(pose);

	if (poseHistory.size() > this->poseHistLen)
	{
		poseHistory.pop();
	}
	myoStateMutex.unlock();
}

myo::Pose MyoState::popPose()
{
	myoStateMutex.lock();
	myo::Pose front = poseHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::queue<myo::Pose> MyoState::getPoseHistory()
{
	myoStateMutex.lock();
	std::queue<myo::Pose> copy = std::queue<myo::Pose>(poseHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::setMyo(MyoDevice *myo)
{ myoHandle = myo; }

const MyoDevice* MyoState::peakMyo()
{ return myoHandle; }