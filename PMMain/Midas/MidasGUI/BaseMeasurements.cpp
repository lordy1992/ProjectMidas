#include "BaseMeasurements.h"
#include <Windows.h>

void BaseMeasurements::setBaseAngles(float roll, float pitch, float yaw)
{
	accessMutex.lock();
	baseRoll = roll;
	basePitch = pitch;
	baseYaw = yaw;
	accessMutex.unlock();
}

void BaseMeasurements::setBaseCursor(unsigned int x, unsigned int y)
{
	accessMutex.lock();
	baseCursorX = x;
	baseCursorY = y;
	accessMutex.unlock();
}

// use Windows API to update cursor position
void BaseMeasurements::updateBaseCursor()
{
	accessMutex.lock();
	POINT cursorPoint;
	if (GetCursorPos(&cursorPoint))
	{
		// success
		baseCursorX = cursorPoint.x;
		baseCursorY = cursorPoint.y;
	}
	accessMutex.unlock();
}

void BaseMeasurements::setScreenSize(unsigned int x, unsigned int y)
{
	accessMutex.lock();
	screenSizeX = x;
	screenSizeY = y;

	int xPixelsPrimScreen = GetSystemMetrics(SM_CXSCREEN);
	int yPixelsPrimScreen = GetSystemMetrics(SM_CYSCREEN);

	int xPixelsAllScreens = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int yPixelsAllScreens = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	int x3 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y3 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentAngles(float roll, float pitch, float yaw)
{
	accessMutex.lock();
	currentRoll = roll;
	currentPitch = pitch;
	currentYaw = yaw;
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentAnglesAsBase()
{
	accessMutex.lock();
	baseRoll = currentRoll;
	basePitch = currentPitch;
	baseYaw = currentYaw;
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentPose(myo::Pose pose)
{
	accessMutex.lock();
	currentPose = pose;
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentState(midasMode state)
{
	accessMutex.lock();
	currentState = state;
	accessMutex.unlock();
}

bool BaseMeasurements::areCurrentValuesValid()
{
	accessMutex.lock();
	// check if current pose is caught up to myoState. if not, invalid.
	if (currentPose == (myoStateHandle->getPoseHistory()).back() && currentState == (controlStateHandle->getMode()))
	{
		accessMutex.unlock();
		return true;
	}
	accessMutex.unlock();
	return false;
}