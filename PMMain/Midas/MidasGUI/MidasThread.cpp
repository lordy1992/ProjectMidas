#include "MidasThread.h"
#include <qdebug.h>

MidasThread::MidasThread()
{
}

MidasThread::~MidasThread()
{

}

void MidasThread::run()
{
    qDebug() << "running spawned thread.";

    return;
}