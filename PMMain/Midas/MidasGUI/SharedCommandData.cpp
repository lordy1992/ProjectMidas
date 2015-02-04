#include "SharedCommandData.h"

void SharedCommandData::addCommand(commandData dat)
{
    commandQueueMutex.lock();
    commandQueue.push(dat);
    commandQueueMutex.unlock();
}

bool SharedCommandData::tryAddCommand(commandData dat)
{
    bool locked = commandQueueMutex.try_lock();
    if (locked) 
    {
        commandQueue.push(dat);
        commandQueueMutex.unlock();
    }

    return locked;
}

bool SharedCommandData::consumeCommand(commandData& dat)
{
    if (commandQueue.empty()) return false;
    commandQueueMutex.lock();
    dat = commandQueue.front();
    commandQueue.pop();
    commandQueueMutex.unlock();

    return true;
}

bool SharedCommandData::tryConsumeCommand(commandData& outCommandData)
{
    bool locked = commandQueueMutex.try_lock();
    if (locked)
    {
        outCommandData = commandQueue.front();
        commandQueue.pop();
        commandQueueMutex.unlock();
    }

    return locked;
}

void SharedCommandData::setVelocity(point velocity)
{
    velocityMutex.lock();
    mouseVelocity = velocity;
    velocityMutex.unlock();
}

bool SharedCommandData::trySetVelocity(point velocity)
{
    bool locked = velocityMutex.try_lock();
    if (locked) {
        mouseVelocity = velocity;
        velocityMutex.unlock();
    }

    return locked;
}

point SharedCommandData::getVelocity()
{
    velocityMutex.lock();
    point velocity = mouseVelocity;
    velocityMutex.unlock();

    return velocity;
}

bool SharedCommandData::tryGetVelocity(point& outVelocity)
{
    bool locked = velocityMutex.try_lock();
    if (locked) {
        outVelocity = mouseVelocity;
        velocityMutex.unlock();
    }

    return locked;
}

void SharedCommandData::setKybdGuiSel(unsigned int kybdGuiSel)
{
    if (kybdGuiSel <= maxKybdGuiSel)
    {
        kybdGuiSelMutex.lock();
        kybdGuiSel = kybdGuiSel;
        kybdGuiSelMutex.unlock();
    }
}

bool SharedCommandData::trySetKybdGuiSel(unsigned int kybdGuiSel)
{
    if (kybdGuiSel <= maxKybdGuiSel)
    {
        bool locked = kybdGuiSelMutex.try_lock();
        if (locked) {
            kybdGuiSel = kybdGuiSel;
            kybdGuiSelMutex.unlock();
        }
        return locked;
    }
    else
    {
        return false;
    }
}

unsigned int SharedCommandData::getKybdGuiSel()
{
    kybdGuiSelMutex.lock();
    unsigned int guiSel = kybdGuiSel;
    kybdGuiSelMutex.unlock();

    return guiSel;
}

bool SharedCommandData::tryGetKybdGuiSel(unsigned int& outKybdGuiSel)
{
    bool locked = kybdGuiSelMutex.try_lock();
    if (locked) {
        outKybdGuiSel = kybdGuiSel;
        kybdGuiSelMutex.unlock();
    }

    return locked;
}

unsigned int SharedCommandData::getKybdGuiSelMax()
{
    kybdGuiSelMutex.lock();
    unsigned int max = maxKybdGuiSel;
    kybdGuiSelMutex.unlock();

    return max;
}

bool SharedCommandData::tryGetKybdGuiSelMax(unsigned int& outMaxKybdGuiSel)
{
    bool locked = kybdGuiSelMutex.try_lock();
    if (locked) {
        outMaxKybdGuiSel = maxKybdGuiSel;
        kybdGuiSelMutex.unlock();
    }

    return locked;
}


void SharedCommandData::setMyoOrientation(orientation_data orientation)
{
    myoOrientationMutex.lock();
    myoOrientation = orientation;
    myoOrientationMutex.unlock();
}

bool SharedCommandData::trySetMyoOrientation(orientation_data orientation)
{
    bool locked = myoOrientationMutex.try_lock();
    if (locked) {
        myoOrientation = orientation;
        myoOrientationMutex.unlock();
    }

    return locked;
}

orientation_data SharedCommandData::getMyoOrientation()
{
    myoOrientationMutex.lock();
    orientation_data orientation = myoOrientation;
    myoOrientationMutex.unlock();

    return orientation;
}


bool SharedCommandData::tryGetMyoOrientation(orientation_data& outMyoOrientation)
{
    bool locked = myoOrientationMutex.try_lock();
    if (locked) {
        outMyoOrientation = myoOrientation;
        myoOrientationMutex.unlock();
    }

    return locked;
}

void SharedCommandData::setKeySelectAngle(int angle)
{
    keySelectAngleMutex.lock();
    keySelectAngle = angle;
    keySelectAngleMutex.unlock();
}

bool SharedCommandData::trySetKeySelectAngle(int angle)
{
    bool locked = keySelectAngleMutex.try_lock();
    if (locked) {
        keySelectAngle = angle;
        keySelectAngleMutex.unlock();
    }

    return locked;
}

int SharedCommandData::getKeySelectAngle()
{
    keySelectAngleMutex.lock();
    int angle = keySelectAngle;
    keySelectAngleMutex.unlock();

    return angle;
}


bool SharedCommandData::tryGetKeySelectAngle(int& outKeySelectAngle)
{
    bool locked = keySelectAngleMutex.try_lock();
    if (locked) {
        outKeySelectAngle = keySelectAngle;
        keySelectAngleMutex.unlock();
    }

    return locked;
}


bool SharedCommandData::isCommandQueueEmpty()
{
    return commandQueue.empty();
}

void SharedCommandData::process()
{
    filterDataMap input = Filter::getInput();
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::END_CHAIN);

    if (input.find(COMMAND_INPUT) != input.end())
    {
        boost::any value = input[COMMAND_INPUT];
        extractCommand(value);
    }
    
    if (input.find(VELOCITY_INPUT) != input.end())
    {
        boost::any value = input[VELOCITY_INPUT];
        extractPoint(value);
    }

    if (input.find(ORIENTATION_INPUT) != input.end())
    {
        boost::any value = input[ORIENTATION_INPUT];
        extractPoint(value);
    }
    if (input.find(ANGLE_INPUT) != input.end())
    {
        extractPoint(input);
    }
}

void SharedCommandData::empty()
{
    commandQueueMutex.lock();
    commandQueue.empty();
    commandQueueMutex.unlock();
}

bool SharedCommandData::tryEmpty()
{
    bool locked = commandQueueMutex.try_lock();

    if (locked)
    {
        commandQueue.empty();
        commandQueueMutex.unlock();
    }

    return locked;
}

void SharedCommandData::extractCommand(boost::any value)
{
    if (value.type() != typeid(commandData)) 
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        commandData data = boost::any_cast<commandData>(value);
        addCommand(data);
    }
}

void SharedCommandData::extractPoint(boost::any value)
{
    if (value.type() != typeid(point))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        point velocity = boost::any_cast<point>(value);
        setVelocity(velocity);
    }
}


void SharedCommandData::extractOrientation(boost::any value)
{
    if (value.type() != typeid(orientation_data))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        orientation_data orientation = boost::any_cast<orientation_data> (value);
        setMyoOrientation(orientation);
    }
}

void SharedCommandData::extractKeySelectAngle(int angle)
{
    setKeySelectAngle(angle);
}




