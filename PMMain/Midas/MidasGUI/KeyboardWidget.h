#ifndef KEYBOARD_WIDGET_H
#define KEYBOARD_WIDGET_H

#include "DraggableWidget.h"
#include "RingData.h"
#include "MidasThread.h"

#define KEYBOARD_RADIUS 200
#define RING_WIDTH 50

class KeyboardWidget : public DraggableWidget
{
    Q_OBJECT

public:
    KeyboardWidget(MidasThread *mainThread, int radius = KEYBOARD_RADIUS, int ringWidth = RING_WIDTH, QWidget *parent = 0);
    QSize sizeHint() const;

    void addWheel(ringData wheel);
    void addWheels(std::vector<ringData> *kybrdRingData);
    void clearWheels();

public slots:
    void updateKeyboard(int, double, bool, bool);
    void handleKeyboardData(int, double);

protected:
    /**
    * The event that is called when the GUI needs to paint itself.
    *
    * @param event The information about the paint event.
    */
    void paintEvent(QPaintEvent *event);

    /**
    * The event that is called when the widget is resized.
    *
    * @param event The information about the resize event.
    */
    void resizeEvent(QResizeEvent *event);

private:
    int getSelectedKeyFromAngle(double angle);
    void drawRing(QPainter &painter, std::vector<ringData::keyboardValue> *ring, int ringInnerRad, bool isSelected);
    void drawKey(QPainter &painter, int ringInnerRad, qreal currAngle, qreal deltaAngle,
        QRectF& outerRect, QRectF& innerRect, ringData::keyboardValue keyDat, QColor& lineColour, int distBetween);

    QPoint position, cursorPos;
    std::vector<ringData> wheels;
    int keyboardRadius, ringWidth;
    int selectedWheel;
    int selectedKey;
    bool outerSelected;

    QString tempDebugText1;
    QString tempDebugText2;
    QString tempDebugText3;
    QString tempDebugText4;
};

#endif