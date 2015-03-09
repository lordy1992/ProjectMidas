#ifndef DISTANCE_WIDGET_H
#define DISTANCE_WIDGET_H

#include "DraggableWidget.h"
#include "MidasCommon.h"
#include "MidasThread.h"
#include "qlabel.h"
#include "qpushbutton.h"

#define BASE_DB -40
//#define DB_PER_M -11.45
#define DB_PER_M -15

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

    /* iOS immediate < 0.5m, near = a little more, far = a few metres 
     * http://www.devfright.com/ibeacons-tutorial-ios-7-clbeaconregion-clbeacon/
     */

    /**
     * Calculate the distance from the give db value returned from myo rssi
     * request. The formula used is based off of measured values.
     *
     * @param db Float value corresponding to the db received from the myo
     * @return Float that represents an approximate distance.
     */
    inline float dbToDist(float db);

};

#endif /* DISTANCE_WIDGET_H */