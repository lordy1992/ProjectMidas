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

void SharedCommandData::setDeltaVolume(float volume)
{
    volumeMutex.lock();
    deltaVolume = volume;
    volumeMutex.unlock();
}

bool SharedCommandData::trySetDeltaVolume(float volume)
{
    bool locked = volumeMutex.try_lock();
    if (locked) {
        deltaVolume = volume;
        volumeMutex.unlock();
    }

    return locked;
}

float SharedCommandData::getDeltaVolume()
{
    volumeMutex.lock();
    float volume = deltaVolume;
    volumeMutex.unlock();

    return volume;
}

bool SharedCommandData::tryGetVolume(float& outVolume)
{
    bool locked = volumeMutex.try_lock();
    if (locked) {
        outVolume = deltaVolume;
        volumeMutex.unlock();
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

float SharedCommandData::getKybdGuiSel()
{
    kybdGuiSelMutex.lock();
    float volume = kybdGuiSel;
    kybdGuiSelMutex.unlock();

    return volume;
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

    if (input.find(DELTA_VOL) != input.end())
    {
        boost::any value = input[DELTA_VOL];
        extractVolume(value);
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

void SharedCommandData::extractVolume(boost::any value)
{
    if (value.type() != typeid(float))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        float deltaVolume = boost::any_cast<float>(value);
        setDeltaVolume(deltaVolume); // Jorden TODO -- Change this to addCommand asap. This should not be like this. Do stuff with owen first, then come back and fix this.
    }
}