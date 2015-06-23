#ifndef _SCD_DIGESTER_H
#define _SCD_DIGESTER_H

#include "SharedCommandData.h"
#include "MidasThread.h"
#include "ControlState.h"
#include "MouseCtrl.h"
#include "kybrdCtrl.h"
#include "RingData.h"
#include "MyoCommon.h"
#include <iostream>

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
#include "myo\myo.hpp"
using namespace myo;
#endif

class SCDDigester
{
public:
    SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, 
        MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl, std::vector<ringData> *kybrdRingData);
    ~SCDDigester();

    void digest();

private:
    void digestKybdCmd(commandData nextCommand);

    void digestKeyboardGUIData(commandData nextCommand);

    int getSelectedKeyFromAngle(double angle, std::vector<ringData::keyboardValue> *ring);

    SharedCommandData *scdHandle;
    MidasThread *threadHandle;
    ControlState *cntrlStateHandle;
    MouseCtrl *mouseCtrl;
    KybrdCtrl *kybrdCtrl;
    int count;

    KeyboardWidget *keyboardWidget;
    std::vector<ringData> *kybrdRingData;
};

#endif /* _SCD_DIGESTER_H */