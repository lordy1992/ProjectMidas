#include "ControlState.h"


ControlState::ControlState(SharedCommandData* SCDHandle)
{
    this->SCDHandle = SCDHandle;
    currentMode = LOCK_MODE;
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
    currentMode = mode;
    SCDHandle->tryEmpty();
    return true;
}

midasMode ControlState::getMode()
{
    return currentMode;
}