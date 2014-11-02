#ifndef _SHARED_COMMAND_DATA_H
#define _SHARED_COMMAND_DATA_H

#include <mutex>
#include <queue>
#include "MidasCommon.h"

class SharedCommandData
{
public:
    SharedCommandData() : mouseRelativeCoordinates() { }
    void addCommand(commandData dat);
    bool tryAddCommand(commandData dat);
    commandData consumeCommand();
    bool tryConsumeCommand(commandData& outCommandData);

    void setRelativeCoordinates(point relativeCoordinates);
    bool trySetRelativeCoordinates(point relativeCoordinates);
    point getRelativeCoordinates();
    bool tryGetRelativeCoordinates(point& outRelativeCoordinates);

private:
    point mouseRelativeCoordinates;
    std::queue<commandData> commandQueue;
    std::mutex commandQueueMutex;
    std::mutex relativeCoordinatesMutex;
};

#endif /* _SHARED_COMMAND_DATA_H */