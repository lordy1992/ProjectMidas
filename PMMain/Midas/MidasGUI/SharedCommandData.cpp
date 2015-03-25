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
    kybdGuiSelMutex.lock();
    if (kybdGuiSel <= maxKybdGuiSel)
    {
        this->kybdGuiSel = kybdGuiSel;
    }
    kybdGuiSelMutex.unlock();
}

bool SharedCommandData::trySetKybdGuiSel(unsigned int kybdGuiSel)
{
    bool locked = kybdGuiSelMutex.try_lock();
    if (locked) {
        if (kybdGuiSel <= maxKybdGuiSel)
        {
            this->kybdGuiSel = kybdGuiSel;
            kybdGuiSelMutex.unlock();
        }
    }
    return locked;
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

void SharedCommandData::setKeySelectAngle(keyboardAngle angle)
{
    keySelectAngleMutex.lock();
    keySelectAngle = angle;
    keySelectAngleMutex.unlock();
}

bool SharedCommandData::trySetKeySelectAngle(keyboardAngle angle)
{
    bool locked = keySelectAngleMutex.try_lock();
    if (locked) {
        keySelectAngle = angle;
        keySelectAngleMutex.unlock();
    }

    return locked;
}

keyboardAngle SharedCommandData::getKeySelectAngle()
{
    keySelectAngleMutex.lock();
    keyboardAngle angle = keySelectAngle;
    keySelectAngleMutex.unlock();

    return angle;
}


bool SharedCommandData::tryGetKeySelectAngle(keyboardAngle& outKeySelectAngle)
{
    bool locked = keySelectAngleMutex.try_lock();
    if (locked) {
        outKeySelectAngle = keySelectAngle;
        keySelectAngleMutex.unlock();
    }

    return locked;
}

float SharedCommandData::getRssi()
{
    rssiMutex.lock();
    float rssi = rssiAVG;
    rssiMutex.unlock();
    return rssi;
}

void SharedCommandData::setRssi(float rssi)
{
    rssiMutex.lock();
    rssiAVG = rssi;
    rssiMutex.unlock();
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

    if (input.find(ANGLE_INPUT) != input.end())
    {
        boost::any value = input[ANGLE_INPUT];
        extractKeySelectAngle(value);
    }

    if (input.find(RSSI_INPUT) != input.end())
    {
        boost::any value = input[RSSI_INPUT];
        extractRssi(value);
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

void SharedCommandData::extractKeySelectAngle(boost::any value)
{
    if (value.type() != typeid(keyboardAngle))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        keyboardAngle angle = boost::any_cast<keyboardAngle> (value);
        setKeySelectAngle(angle);
    }
}

void SharedCommandData::extractRssi(boost::any value)
{
    if (value.type() != typeid(float))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        float rssi = boost::any_cast<float> (value);
        setRssi(rssi);
    }
}