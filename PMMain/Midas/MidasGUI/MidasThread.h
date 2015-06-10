#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"
#include "ProfileManager.h"

class MainGUI;

class MidasThread : public QThread
{
    Q_OBJECT

public:
    ~MidasThread();
	MidasThread();

    void setMainGuiHandle(MainGUI *mainGui);
    void setProfileManagerHandle(ProfileManager *profileManager);
    void run();

private:
    MainGUI *mainGui;
    ProfileManager *profileManager;

signals:
    void emitVeloc(int, int);

    void emitDebugInfo(int, int);

    void emitDisconnect(bool);
};

#endif // MIDASTHREAD_H
