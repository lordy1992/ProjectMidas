#ifndef _GESTURE_FILTER_H
#define _GESTURE_FILTER_H

#include "myo\myo.hpp"
#include "ControlState.h"
#include "Filter.h"

#define GESTURE_INPUT "gesture"

class GestureFilter : public Filter
{
public:
    GestureFilter(ControlState* controlState, clock_t timeDel);
    ~GestureFilter();

    void process();

private:
    commandData translateGesture(myo::Pose::Type pose);

    myo::Pose::Type lastPoseType;
    ControlState* controlStateHandle;
    clock_t timeDelta;
    clock_t lastTime;

};

#endif /* _GESTURE_FILTER_H */