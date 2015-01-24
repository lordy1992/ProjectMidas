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

void DraggableWidget::formBoxLabel(QLabel *label)
{
    label->setEnabled(false);
    label->setFrameShape(QFrame::Box);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setBackgroundRole(QPalette::Base);
    label->setAlignment(Qt::AlignCenter);
    label->setAutoFillBackground(true);
    label->setFixedSize(GRID_ELEMENT_SIZE, GRID_ELEMENT_SIZE);
    label->setScaledContents(true);
}