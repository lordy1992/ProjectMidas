#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>
#include <string>

class MidasThread : public QThread
{
    Q_OBJECT

public:
    MidasThread();
    ~MidasThread();

    void run();

    void emitInfo();

    void threadEmitString(std::string str);

private:

signals:
    void outputCount(int);

    void emitString(std::string);
    
};

#endif // MIDASTHREAD_H