#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"

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

private:

signals:
    void outputCount(int);
    void emitString(std::string);
    void emitStateString(std::string);
    void emitVeloc(int, int);
    void emitAddSequence(std::string, std::vector<sequenceImageSet>);
    void emitAdvanceSequence(int);
    void emitUpdateSequences();
    
};

#endif // MIDASTHREAD_H
