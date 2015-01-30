#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"

class MainGUI;

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread();
    ~MidasThread();

    void setMainGuiHandle(MainGUI *mainGui);
    void run();

private:
    MainGUI *mainGui;

signals:
    void emitVeloc(int, int);
    
};

#endif // MIDASTHREAD_H
