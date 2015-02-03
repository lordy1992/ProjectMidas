#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "RingData.h"
#include "MyoCommon.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread(SequenceDisplayer *sequenceDisplayerGui, InfoIndicator *infoIndicator);
    ~MidasThread();

    void run();

    void emitInfo();

    void threadEmitString(std::string str);
    void threadEmitStateString(std::string str);

private:
    SequenceDisplayer *sequenceDisplayer;
    InfoIndicator *infoIndicator;

signals:
    void outputCount(int);
    void emitString(std::string);
    void emitStateString(std::string);
    void emitVeloc(int, int);
    
};

#endif // MIDASTHREAD_H
