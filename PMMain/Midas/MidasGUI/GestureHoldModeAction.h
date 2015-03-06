#ifndef GESTURE_HOLD_MODE_ACTION_HPP
#define GESTURE_HOLD_MODE_ACTION_HPP

#include "MidasCommon.h"
#include <unordered_map>

typedef struct angleData {
    bool operator==(const angleData& ad)
    {
        if (this->anglePositive == ad.anglePositive &&
            this->angleType == ad.angleType)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int hash()
    {
        return 4 * angleType + (anglePositive ? 1 : 0);
    }
    
    enum AngleType {
        ROLL,
        PITCH,
        YAW
    };
    AngleType angleType;
    bool anglePositive;
} angleData;

class GestureHoldModeAction {
public:
    GestureHoldModeAction();
    GestureHoldModeAction(unsigned int minIntervalLen);

    bool addToActionMap(angleData ad, kybdCmds command);

    commandData getAction(angleData ad);

private:
    // TODO - actually care about this type. In the meantime, coding Interval_delta since it's easiest.
    // enum holdModeActionType {
    //     INTERVAL_DELTA, // if in a given interval a signal is +/-, an action will ensue
    //     ABSOLUTE_DELTA_NUM_SIGS, // Based on the delta, ensure the number of +/- actions is at a certain value (repeatable change)
    //     ABSOLUTE_DELTA_VELOCITY // Based on delta, continue spewing actions
    // };

    std::unordered_map<int, commandData> actionMap;

    unsigned int minIntervalLen; // between sampling signals and sending actions // TODO - make this useful
    
};

#endif /* GESTURE_HOLD_MODE_ACTION_HPP */