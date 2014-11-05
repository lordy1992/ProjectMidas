#ifndef _SHARED_COMMAND_DATA_H
#define _SHARED_COMMAND_DATA_H

#include <mutex>
#include <queue>
#include "MidasCommon.h"
#include "Filter.h"

#define COMMAND_INPUT "command"
#define COORD_INPUT "relative_coordinates"
#define MODE_INPUT "mode"

class SharedCommandData : public Filter
{
public:
    SharedCommandData() : Filter(), mouseRelativeCoordinates(), currentMode(MOUSE_MODE) {}
    void addCommand(commandData dat);
    bool tryAddCommand(commandData dat);
    bool consumeCommand(commandData& dat);
    bool tryConsumeCommand(commandData& outCommandData);

    void setRelativeCoordinates(point relativeCoordinates);
    bool trySetRelativeCoordinates(point relativeCoordinates);
    point getRelativeCoordinates();
    bool tryGetRelativeCoordinates(point& outRelativeCoordinates);

    void setMode(midasMode mode);
    bool trySetMode(midasMode mode);
    midasMode getMode();

    bool isCommandQueueEmpty();
    void process();

private:
    point mouseRelativeCoordinates;
    std::queue<commandData> commandQueue;
    midasMode currentMode;
    std::mutex commandQueueMutex;
    std::mutex relativeCoordinatesMutex;

    void extractCommand(boost::any value);
    void extractPoint(boost::any value);
    void extractMode(boost::any value);
};

#endif /* _SHARED_COMMAND_DATA_H */