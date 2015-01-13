#ifndef DRAGGABLE_WIDGET_H
#define DRAGGABLE_WIDGET_H

#include <QtWidgets/QWidget.h>

class DraggableWidget
{
public:
    DraggableWidget(QWidget *parent = 0);
    ~DraggableWidget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
private:
    QPoint position;
};

#endif