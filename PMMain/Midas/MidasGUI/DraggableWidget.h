#ifndef DRAGGABLE_WIDGET_H
#define DRAGGABLE_WIDGET_H

#include <QtWidgets/QWidget.h>
#include <qlabel.h>

#define GRID_ELEMENT_SIZE   48

/**
 * This class represents a Widget that can be dragged using the mouse, by left-clicking the widget
 * and dragging.
 */
class DraggableWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a DraggableWidget with a given parent (or no parent, if NULL), and the
     * specified flags f.
     *
     * @param parent The parent widget.
     * @param f The window flags.
     */
    DraggableWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~DraggableWidget();

protected:
    /**
     * The event handler function that is called when the widget is clicked on by the mouse.
     *
     * @param event The mouse event structure, with information about the mouse press.
     */
    virtual void mousePressEvent(QMouseEvent *event);

    /**
     * The event handler function that is called when the mouse moves over the widget.
     */
    virtual void mouseMoveEvent(QMouseEvent *event);

    void formBoxLabel(QLabel *label);
    
private:
    QPoint position;
};

#endif