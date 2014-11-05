#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include "MidasCommon.h"

class mouseCtrl
{
public:
    mouseCtrl();

    void sendCommand(mouseCmds mouseCmd, bool releaseIfClick = true);

private:
    MOUSEINPUT mi;
};

#endif /* _MOUSE_CONTROLLER_H */