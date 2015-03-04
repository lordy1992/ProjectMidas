#ifndef DISTANCE_WIDGET_H
#define DISTANCE_WIDGET_H

#include "DraggableWidget.h"
#include "MidasCommon.h"
#include "MidasThread.h"

#define BASE_DB -50
#define DB_PER_M -11.45

class DistanceWidget : public QWidget
{
    Q_OBJECT
public:
    DistanceWidget(MidasThread *mainThread, QWidget *parent = 0);
    
public slots:
    void updateDistanceLabel(float db);

private:
     
    QLabel *distanceLabel;
    float distance;

};

#endif /* DISTANCE_WIDGET_H */