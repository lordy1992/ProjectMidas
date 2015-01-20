#ifndef MOUSE_INDICATOR_H
#define MOUSE_INDICATOR_H

#include "DraggableWidget.h"
#include "MidasThread.h"

class MouseIndicator : public DraggableWidget
{
    Q_OBJECT

public:
    MouseIndicator(MidasThread *mainThread, int widgetWidth = 100, int widgetHeight = 100, QWidget *parent = 0);
    QSize sizeHint() const;

    // For testing handleUpdateCursorPos, until we plug it into the Myo code.
    void keyPressEvent(QKeyEvent *e);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QPoint position, cursorPos;
    int indWidth, indHeight;
    MidasThread *mainThread;

public slots:
    void handleUpdateCursorPos(int percentX, int percentY);
};

#endif MOUSE_INDICATOR_H