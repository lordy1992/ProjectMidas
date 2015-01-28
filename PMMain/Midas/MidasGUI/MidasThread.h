#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "RingData.h"

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread();
    ~MidasThread();

    void run();

    void emitInfo();

    void threadEmitString(std::string str);
    void threadEmitStateString(std::string str);
    std::vector<ringData> readKeyboardSetupFile();
private:

signals:
    void outputCount(int);

    void emitString(std::string);
    void emitStateString(std::string);

    void emitXVeloc(int);
    void emitYVeloc(int);
    
};

#endif // MIDASTHREAD_H
