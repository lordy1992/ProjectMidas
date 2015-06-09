#include "MidasThread.h"
#include "MidasMain.h"
#include "MainGUI.h"
#include <qdebug.h>
#include <Windows.h>


MidasThread::MidasThread()
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
    midasMain(this, mainGui, profileManager);
}

