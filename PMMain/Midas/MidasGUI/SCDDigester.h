#ifndef _SCD_DIGESTER_H
#define _SCD_DIGESTER_H

#include "SharedCommandData.h"
#include "MidasThread.h"
#include "ControlState.h"
#include "MyoState.h"
#include "MouseCtrl.h"
#include "kybrdCtrl.h"
#include "MyoCommon.h"
#include "KeyboardContoller.h"
#include "CommandData.h"
#include "ProfileManager.h"
#include "ProfileSignaller.h"
#include <iostream>

#ifdef BUILD_KEYBOARD
#include "RingData.h"
#endif

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
#ifdef BUILD_KEYBOARD
	SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MyoState *myoStateHandle,
		MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl, KeyboardController *keyboardController, ProfileManager* profileManagerHandle, std::vector<ringData> *kybrdRingData);
#else
	SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MyoState *myoStateHandle,
		MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl, KeyboardController *keyboardController, ProfileManager* profileManagerHandle);
#endif
    ~SCDDigester();

    void digest();

private:
    void digestKybdCmd(CommandData nextCommand);

	void digestProfileChange(CommandData nextCmd);

#ifdef BUILD_KEYBOARD
    void digestKeyboardGUIData(CommandData nextCommand);

	int getSelectedKeyFromAngle(double angle, std::vector<ringData::keyboardValue> *ring);

	KeyboardWidget *keyboardWidget;
	std::vector<ringData> *kybrdRingData;
#endif

	KeyboardController *keyboardController;
    SharedCommandData *scdHandle;
    MidasThread *threadHandle;
    ControlState *cntrlStateHandle;
	MyoState *myoStateHandle;
    MouseCtrl *mouseCtrl;
    KybrdCtrl *kybrdCtrl;
    int count;

	ProfileManager *pm;
};

#endif /* _SCD_DIGESTER_H */