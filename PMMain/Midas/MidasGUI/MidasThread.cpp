#include "MidasThread.h"
#include "MidasMain.h"
#include <qdebug.h>
#include <Windows.h>
#include "RingData.h"

MidasThread::MidasThread(SequenceDisplayer *sequenceDisplayerGui, InfoIndicator *infoIndicator)
    : sequenceDisplayer(sequenceDisplayerGui), infoIndicator(infoIndicator)
{
}

MidasThread::~MidasThread()
{
}

void MidasThread::run()
{
    qDebug() << "running spawned thread.";
    threadEmitString("test within run func.");
    midasMain(this, sequenceDisplayer, infoIndicator);

    for (int i = 0; i < 100000; i++)
    {
        emit outputCount(i);
        Sleep(25);
    }

    return;
}

void MidasThread::emitInfo()
{
    emit outputCount(888);
}

void MidasThread::threadEmitString(std::string str)
{
    emit emitString(str);
}

void MidasThread::threadEmitStateString(std::string str)
{
    emit emitStateString(str);
}

