#include "ControlState.h"


ControlState::ControlState(SharedCommandData* SCDHandle)
{
    this->SCDHandle = SCDHandle;
    currentMode = LOCK_MODE;
	currentProfile = "default";
}


ControlState::~ControlState()
{
}


void ControlState::setMode(midasMode mode)
{
    // Note that empty() uses a command Queue Mutex to protect
    // the SharedCommandData integrity.
    currentMode = mode;
    SCDHandle->empty();
}

bool ControlState::trySetMode(midasMode mode)
{
    // Note that tryEmpty() uses a command Queue Mutex to protect
    // the SharedCommandData integrity.
    
	if (SCDHandle->tryEmpty()) {
		currentMode = mode;
	}
	else {
		return false;
	}
    return true;
}

midasMode ControlState::getMode()
{
    return currentMode;
}

void ControlState::setProfile(std::string profile)
{
	//profileMutex.lock();
	currentProfile = profile;
	//profileMutex.unlock();
}

std::string ControlState::getProfile()
{
	return currentProfile;
}