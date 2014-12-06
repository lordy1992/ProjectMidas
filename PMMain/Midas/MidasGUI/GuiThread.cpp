#include "GuiThread.h"
#include <qdebug.h>

GUIThread::GUIThread(QApplication *app, MidasGUI *window)
    : app(app), window(window)
{

}

GUIThread::~GUIThread()
{

}


void GUIThread::run()
{
    qDebug() << "running spawned thread.";
    window->show();
    app->exec();

    qDebug() << "returning from spawned thread.";
    return;
}