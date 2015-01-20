#ifndef _SCD_DIGESTER_H
#define _SCD_DIGESTER_H

#include "SharedCommandData.h"
#include "MidasThread.h"
#include "ControlState.h"
#include "MouseCtrl.h"
#include "kybrdCtrl.h"
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
	SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MouseCtrl *mouseCtrl, KybrdCtrl *kybrdCtrl);
	~SCDDigester();

	void digest();

private:
	SharedCommandData *scdHandle;
	MidasThread *threadHandle;
	ControlState *cntrlStateHandle;
	MouseCtrl *mouseCtrl;
	KybrdCtrl *kybrdCtrl;
	int count;
};

#endif /* _SCD_DIGESTER_H */