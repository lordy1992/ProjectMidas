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

void MidasThread::run()
{
    qDebug() << "running spawned thread.";
    threadEmitString("test within run func.");
    midasMain(this);

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
