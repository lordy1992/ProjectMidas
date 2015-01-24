#pragma once

#include "QObject.h"
#include "MyoCommon.h"

class GestureSignaller : public QObject
{
    Q_OBJECT

public:
    GestureSignaller();
    ~GestureSignaller(); 

    bool getShowAll();

    public slots:
    void handleShowAllToggle(bool showAll);

signals:
    void emitRegisterSequence(int, QString, std::vector<sequenceImageSet>);
    void emitShowSequences(std::vector<sequenceProgressData>);

    void emitStateString(QString);

private:
    bool showAll;
};

