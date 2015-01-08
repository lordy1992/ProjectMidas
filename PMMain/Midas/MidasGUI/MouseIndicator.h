#ifndef MOUSE_INDICATOR_H
#define MOUSE_INDICATOR_H

#include <QtWidgets/QWidget.h>

class MouseIndicator : public QWidget
{
    Q_OBJECT

public:
    MouseIndicator(int widgetWidth = 100, int widgetHeight = 100, QWidget *parent = 0);
    QSize sizeHint() const;

    // For testing handleUpdateCursorPos, until we plug it into the Myo code.
    void keyPressEvent(QKeyEvent *e);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint position, cursorPos;
    int indWidth, indHeight;

public slots:
    void handleUpdateCursorPos(int percentX, int percentY);
};

#endif MOUSE_INDICATOR_H