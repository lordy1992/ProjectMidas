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

private:
    MainGUI *mainGui;
    std::vector<ringData> *kybrdRingData;

signals:
    void emitVeloc(int, int);

    void emitUpdateKeyboard(int, double, bool, bool);  // kybdGUISel, angle, center, held
    
};

#endif // MIDASTHREAD_H
