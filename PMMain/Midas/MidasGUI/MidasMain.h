#ifndef MIDAS_MAIN_H
#define MIDAS_MAIN_H

#include "MidasThread.h"
#include "ProfileManager.h"

int midasMain(MidasThread *threadHandle, MainGUI *mainGui, ProfileManager *pm, std::vector<ringData> *kybrdRingData);

#endif /* MIDAS_MAIN_H */