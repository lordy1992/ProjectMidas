#include "MidasThread.h"
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

    for (int i = 0; i < 100000; i++)
    {
        emit outputCount(i);
        Sleep(25);
    }

    return;
}