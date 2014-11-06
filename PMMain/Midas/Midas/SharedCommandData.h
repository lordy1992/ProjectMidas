#ifndef _SHARED_COMMAND_DATA_H
#define _SHARED_COMMAND_DATA_H

#include <mutex>
#include <queue>
#include "MidasCommon.h"
#include "Filter.h"

#define COMMAND_INPUT "command"
#define VELOCITY_INPUT "velocity"
#define MODE_INPUT "mode"

class SharedCommandData : public Filter
{
public:
    SharedCommandData() : Filter(), mouseVelocity(), currentMode(MOUSE_MODE) {}
    void addCommand(commandData dat);
    bool tryAddCommand(commandData dat);
    bool consumeCommand(commandData& dat);
    bool tryConsumeCommand(commandData& outCommandData);

    void setVelocity(point velocity);
    bool trySetVelocity(point velocity);
    point getVelocity();
    bool tryGetVelocity(point& outVelocity);

    void setMode(midasMode mode);
    bool trySetMode(midasMode mode);
    midasMode getMode();

    bool isCommandQueueEmpty();
    void process();

private:
    point mouseVelocity;
    std::queue<commandData> commandQueue;
    midasMode currentMode;
    std::mutex commandQueueMutex;
    std::mutex velocityMutex;

    void extractCommand(boost::any value);
    void extractPoint(boost::any value);
    void extractMode(boost::any value);
};

#endif /* _SHARED_COMMAND_DATA_H */