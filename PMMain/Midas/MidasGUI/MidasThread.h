#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"
#include "RingData.h"

class MainGUI;
class KeyboardWidget;

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread(std::vector<ringData> *kybrdRingData);
    ~MidasThread();

    void setMainGuiHandle(MainGUI *mainGui);
    void run();
    std::vector<ringData>* getKybrdRingData();

private:
    MainGUI *mainGui;
    std::vector<ringData> *kybrdRingData;

signals:
    void emitVeloc(int, int);

    void emitUpdateKeyboard(int, int, bool, bool);  // kybdGUISel, keySelect, center, held

    void emitDebugInfo(int, int);
};

#endif // MIDASTHREAD_H
