#ifndef DRAGGABLE_WIDGET_H
#define DRAGGABLE_WIDGET_H

#include <QtWidgets/QWidget.h>

class DraggableWidget : public QWidget
{
    Q_OBJECT

public:
    DraggableWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~DraggableWidget();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    
private:
    QPoint position;
};

#endif