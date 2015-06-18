#include "MidasThread.h"
#include "MidasMain.h"
#include "MainGUI.h"
#include <qdebug.h>
#include <Windows.h>
#ifdef BUILD_KEYBOARD
#include "KeyboardWidget.h"
#endif

#ifdef BUILD_KEYBOARD
MidasThread::MidasThread(std::vector<ringData> *kybrdRingData) : kybrdRingData(kybrdRingData)
#else
MidasThread::MidasThread()
#endif
{
}

MidasThread::~MidasThread()
{
}

void MidasThread::setMainGuiHandle(MainGUI *mainGui)
{
    this->mainGui = mainGui;
}

void MidasThread::setProfileManagerHandle(ProfileManager *profileManager)
{
    this->profileManager = profileManager;
}

void MidasThread::run()
{
#ifdef BUILD_KEYBOARD
	midasMain(this, mainGui, profileManager, kybrdRingData);
#else
	midasMain(this, mainGui, profileManager);
#endif
}

#ifdef BUILD_KEYBOARD
std::vector<ringData>* MidasThread::getKybrdRingData()
{
	return kybrdRingData;
}
#endif

