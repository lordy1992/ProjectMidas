#include "MidasThread.h"
#include "MidasMain.h"
#include "MainGUI.h"
#include "KeyboardWidget.h"
#include <qdebug.h>
#include <Windows.h>


MidasThread::MidasThread(std::vector<ringData> *kybrdRingData) : kybrdRingData(kybrdRingData)
{
}

MidasThread::~MidasThread()
{
}

std::vector<ringData>* MidasThread::getKybrdRingData()
{
    return kybrdRingData;
}

void MidasThread::setMainGuiHandle(MainGUI *mainGui)
{
    this->mainGui = mainGui;
}

void MidasThread::run()
{
    midasMain(this, mainGui, kybrdRingData);
}

