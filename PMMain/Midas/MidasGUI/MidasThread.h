#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"
#include "RingData.h"
#include "ProfileManager.h"

class MainGUI;
class KeyboardWidget;

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread(std::vector<ringData> *kybrdRingData);
    ~MidasThread();

    void setMainGuiHandle(MainGUI *mainGui);
    void setProfileManagerHandle(ProfileManager *profileManager);
    void run();
    std::vector<ringData>* getKybrdRingData();

private:
    MainGUI *mainGui;
    ProfileManager *profileManager;
    std::vector<ringData> *kybrdRingData;

signals:
    void emitVeloc(int, int);

    void emitUpdateKeyboard(int, double, bool, bool);  // kybdGUISel, angle, center, held

    void emitDebugInfo(int, int);

    void emitRssi(float);

    void emitDisconnect(bool);
};

#endif // MIDASTHREAD_H
