#ifndef GUITHREAD_H
#define GUITHREAD_H

#include <QThread>
#include "MidasGui.h"

class GUIThread : public QThread
{
    Q_OBJECT

public:
    GUIThread(QObject *parent, QApplication *app, MidasGUI *window);
    ~GUIThread();

    void run();

private:
    QApplication *app;
    MidasGUI *window;
};

#endif // GUITHREAD_H
