#ifndef DRAGGABLE_WIDGET_H
#define DRAGGABLE_WIDGET_H

#include <QtWidgets/QWidget.h>
#include <qlabel.h>

#define GRID_ELEMENT_SIZE   48

class DraggableWidget : public QWidget
{
    Q_OBJECT

public:
    DraggableWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~DraggableWidget();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    void formBoxLabel(QLabel *label);
    
private:
    QPoint position;
};

#endif