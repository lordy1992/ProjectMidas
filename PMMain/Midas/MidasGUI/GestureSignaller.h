#pragma once

#include "QObject.h"
#include "MyoCommon.h"

/**
 * The GestureSignaller class handles the communication between the Midas sequence logic
 * and the SequenceDisplayer GUI.
 */
class GestureSignaller : public QObject
{
    Q_OBJECT

public:
    GestureSignaller();
    ~GestureSignaller(); 

signals:
    void emitRegisterSequence(int, QString, std::vector<sequenceImageSet>);
    void emitShowSequences(std::vector<sequenceProgressData>);
};

