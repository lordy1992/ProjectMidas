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

bool SharedCommandData::getIsConnected()
{
    isConnectedMutex.lock();
    bool connected = isConnected;
    isConnectedMutex.unlock();
    return connected;
}

void SharedCommandData::setIsConnected(bool connected)
{
    isConnectedMutex.lock();
    isConnected = connected;
    isConnectedMutex.unlock();
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

    if (input.find(ISCONNECTED_INPUT) != input.end())
    {
        boost::any value = input[ISCONNECTED_INPUT];
        extractIsConnected(value);
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

void SharedCommandData::extractIsConnected(boost::any value)
{
    if (value.type() != typeid(bool))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        bool isConnected = boost::any_cast<bool> (value);
        setIsConnected(isConnected);
    }
}

void SharedCommandData::setDelta(point delta)
{
	mouseDeltaMutex.lock();
	mouseDelta = delta;
	mouseDeltaMutex.unlock();
}

bool SharedCommandData::trySetDelta(point delta)
{
	bool locked = mouseDeltaMutex.try_lock();
	if (locked) {
		mouseDelta = delta;
		mouseDeltaMutex.unlock();
	}

	return locked;
}

point SharedCommandData::getDelta()
{
	mouseDeltaMutex.lock();
	point delta = mouseDelta;
	mouseDeltaMutex.unlock();

	return delta;
}

bool SharedCommandData::tryGetDelta(point& outDelta)
{
	bool locked = mouseDeltaMutex.try_lock();
	if (locked) {
		outDelta = mouseDelta;
		mouseDeltaMutex.unlock();
	}

	return locked;
}