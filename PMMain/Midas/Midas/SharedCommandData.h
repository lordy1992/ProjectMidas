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
    SharedCommandData() : Filter(), mouseVelocity() {}
    void addCommand(commandData dat);
    bool tryAddCommand(commandData dat);
    bool consumeCommand(commandData& dat);
    bool tryConsumeCommand(commandData& outCommandData);

    void setVelocity(point velocity);
    bool trySetVelocity(point velocity);
    point getVelocity();
    bool tryGetVelocity(point& outVelocity);

    bool isCommandQueueEmpty();
    void process();

    void empty();
    void tryEmpty();

private:
    point mouseVelocity;
    std::queue<commandData> commandQueue;
    std::mutex commandQueueMutex;
    std::mutex velocityMutex;

    void extractCommand(boost::any value);
    void extractPoint(boost::any value);
    void extractMode(boost::any value);
};

#endif /* _SHARED_COMMAND_DATA_H */