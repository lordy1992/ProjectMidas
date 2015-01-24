#ifndef MOUSE_INDICATOR_H
#define MOUSE_INDICATOR_H

#include "DraggableWidget.h"
#include "MidasThread.h"
#include "MidasCommon.h"

class MouseIndicator : public DraggableWidget
{
    Q_OBJECT

public:
    MouseIndicator(MidasThread *mainThread, int deadZoneRad, int widgetWidth = MOUSE_INDICATOR_SIZE, int widgetHeight = MOUSE_INDICATOR_SIZE, QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QPoint position, cursorPos;
    int indWidth, indHeight;
    int deadZoneRadius;
    MidasThread *mainThread;

public slots:
    void handleUpdateCursorPos(int percentX, int percentY);
};

#endif MOUSE_INDICATOR_H