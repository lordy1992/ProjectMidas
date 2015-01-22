#pragma once

#include "QObject.h"
#include "MyoCommon.h"

class GestureSignaller : public QObject
{
    Q_OBJECT

public:
    GestureSignaller();
    ~GestureSignaller(); 

signals:
    void emitRegisterSequence(int, std::string, std::vector<sequenceImageSet>);
    void emitShowSequences(std::vector<sequenceProgressData>);
};

