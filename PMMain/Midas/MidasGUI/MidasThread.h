#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread();
    ~MidasThread();

    void run();

    void emitInfo();

private:

signals:
    void outputCount(int);
    
};

#endif // MIDASTHREAD_H
