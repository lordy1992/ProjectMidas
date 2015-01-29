#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>
#include "MyoCommon.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread();
    ~MidasThread();

    void setDisplayHandles(SequenceDisplayer *sequenceDisplayerGui, InfoIndicator *infoIndicatorGui);
    void run();

private:
    SequenceDisplayer *sequenceDisplayer;
    InfoIndicator *infoIndicator;

signals:
    void emitVeloc(int, int);
    
};

#endif // MIDASTHREAD_H
