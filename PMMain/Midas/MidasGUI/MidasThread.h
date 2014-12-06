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

private:
    
};

#endif // MIDASTHREAD_H
