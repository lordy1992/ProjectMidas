#include "DraggableWidget.h"
#include <QEvent.h>

DraggableWidget::DraggableWidget(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
}

DraggableWidget::~DraggableWidget()
{
}

void DraggableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        position = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void DraggableWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - position);
        event->accept();
    }
}