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

void SharedCommandData::setRelativeCoordinates(point relativeCoordinates)
{
    relativeCoordinatesMutex.lock();
    mouseRelativeCoordinates = relativeCoordinates;
    relativeCoordinatesMutex.unlock();
}

bool SharedCommandData::trySetRelativeCoordinates(point relativeCoordinates)
{
    bool locked = relativeCoordinatesMutex.try_lock();
    if (locked) {
        mouseRelativeCoordinates = relativeCoordinates;
        relativeCoordinatesMutex.unlock();
    }

    return locked;
}

point SharedCommandData::getRelativeCoordinates()
{
    relativeCoordinatesMutex.lock();
    point relativeCoordinates = mouseRelativeCoordinates;
    relativeCoordinatesMutex.unlock();

    return relativeCoordinates;
}

bool SharedCommandData::tryGetRelativeCoordinates(point& outRelativeCoordinates)
{
    bool locked = relativeCoordinatesMutex.try_lock();
    if (locked) {
        outRelativeCoordinates = mouseRelativeCoordinates;
        relativeCoordinatesMutex.unlock();
    }

    return locked;
}

void SharedCommandData::setMode(midasMode mode)
{
    commandQueueMutex.lock();
    currentMode = mode;
    commandQueue.empty();
    commandQueueMutex.unlock();
}

bool SharedCommandData::trySetMode(midasMode mode)
{
    bool locked = commandQueueMutex.try_lock();
    if (locked) {
        currentMode = mode;
        commandQueue.empty();
        commandQueueMutex.unlock();
    }

    return locked;
}

midasMode SharedCommandData::getMode()
{
    return currentMode;
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
    
    if (input.find(COORD_INPUT) != input.end())
    {
        boost::any value = input[COORD_INPUT];
        extractPoint(value);
    }

    if (input.find(MODE_INPUT) != input.end())
    {
        boost::any value = input[MODE_INPUT];
        extractMode(value);
    }
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
        point relativeCoordinates = boost::any_cast<point>(value);
        setRelativeCoordinates(relativeCoordinates);
    }
}

void SharedCommandData::extractMode(boost::any value)
{
    if (value.type() != typeid(midasMode))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        midasMode mode = boost::any_cast<midasMode>(value);
        setMode(mode);
    }
}