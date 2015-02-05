#ifndef POSE_DISPLAYER_H
#define POSE_DISPLAYER_H

#include "DraggableWidget.h"
#include "MidasCommon.h"
#include "MyoCommon.h"
#include <QGridLayout.h>
#include <qpushbutton.h>

Q_DECLARE_METATYPE(Pose)

class PoseDisplayer : public QWidget
{
    Q_OBJECT

public:
    PoseDisplayer(int widgetWidth = GRID_ELEMENT_SIZE, int widgetHeight = GRID_ELEMENT_SIZE, QWidget *parent = 0);
    ~PoseDisplayer();
    QSize sizeHint() const;


protected:
    void resizeEvent(QResizeEvent *event);

private:
    QPoint position;
    int indWidth, indHeight;
    QGridLayout *layout;

    QPixmap posePix;
    QLabel *tempLabel;

public slots:
    void handlePoseEnum(QString poseEnum);
};

#endif POSE_DISPLAYER_H