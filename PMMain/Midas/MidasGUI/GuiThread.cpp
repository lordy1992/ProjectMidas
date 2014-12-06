#include "GuiThread.h"
#include <qdebug.h>

GUIThread::GUIThread(QObject *parent, QApplication *app, MidasGUI *window)
    : QThread(parent), app(app), window(window)
{

}

GUIThread::~GUIThread()
{

}


void GUIThread::run()
{
    qDebug() << "running spawned thread.";
}