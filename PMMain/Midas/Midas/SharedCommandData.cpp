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