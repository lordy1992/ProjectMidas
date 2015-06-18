#ifndef MIDAS_MAIN_H
#define MIDAS_MAIN_H

#include "MidasThread.h"
#include "ProfileManager.h"

#ifdef BUILD_KEYBOARD
int midasMain(MidasThread *threadHandle, MainGUI *mainGui, ProfileManager *pm, std::vector<ringData> *kybrdRingData);
#else
int midasMain(MidasThread *threadHandle, MainGUI *mainGui, ProfileManager *pm);
#endif

#endif /* MIDAS_MAIN_H */