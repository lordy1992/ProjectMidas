#ifndef _CONTROL_STATE_H
#define _CONTROL_STATE_H

#include <mutex>
#include "MidasCommon.h"
#include "SharedCommandData.h"

class ControlState
{
public:
    /* Basic Construction/Destruction */
    ControlState(SharedCommandData* SCDHandle);
    ~ControlState();

    void setMode(midasMode mode);
    bool trySetMode(midasMode mode);
    midasMode getMode();

private:
    midasMode currentMode;
    SharedCommandData *SCDHandle;
};

#endif /* _CONTROL_STATE_H */