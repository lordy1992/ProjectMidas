#include "MidasThread.h"
#include "MidasMain.h"
#include <qdebug.h>
#include <Windows.h>

MidasThread::MidasThread()
{
}

MidasThread::~MidasThread()
{
}

void MidasThread::setDisplayHandles(SequenceDisplayer *sequenceDisplayerGui, InfoIndicator *infoIndicatorGui)
{
    sequenceDisplayer = sequenceDisplayerGui;
    infoIndicator = infoIndicatorGui;
}

void MidasThread::run()
{
    midasMain(this, sequenceDisplayer, infoIndicator);
}
