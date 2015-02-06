#ifndef POSE_DISPLAYER_H
#define POSE_DISPLAYER_H

#include "DraggableWidget.h"
#include "MidasCommon.h"
#include "MyoCommon.h"
#include <QGridLayout.h>
#include <qpushbutton.h>

// Q_DECLARE_METATYPE(sequenceImageSet) // TODO - this would be better than std::vector<sequenceImageSet>, but is not working immediately

class PoseDisplayer : public QWidget
{
    Q_OBJECT

public:
    PoseDisplayer(int widgetWidth, int widgetHeight, QWidget *parent = 0);
    ~PoseDisplayer();
    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initPoseImgMap();

    QPoint position;
    int indWidth, indHeight;
    QGridLayout *layout;

    QLabel *poseImgLabel;

public slots:
void handlePoseImages(std::vector<sequenceImageSet> poseImages);
};

#endif POSE_DISPLAYER_H