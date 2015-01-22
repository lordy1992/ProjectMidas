#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"
#include "SequenceDisplayer.h"

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread(SequenceDisplayer *sequenceDisplayerGui);
    ~MidasThread();

    void run();

    void emitInfo();

    void threadEmitString(std::string str);
    void threadEmitStateString(std::string str);

private:
    SequenceDisplayer *sequenceDisplayer;

signals:
    void outputCount(int);
    void emitString(std::string);
    void emitStateString(std::string);
    void emitVeloc(int, int);
    
};

#endif // MIDASTHREAD_H
