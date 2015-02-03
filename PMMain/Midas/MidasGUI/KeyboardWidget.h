#ifndef KEYBOARD_WIDGET_H
#define KEYBOARD_WIDGET_H

#include "DraggableWidget.h"

#define KEYBOARD_RADIUS 200

class KeyboardWidget : public DraggableWidget
{
    Q_OBJECT

public:
    KeyboardWidget(int radius = KEYBOARD_RADIUS, QWidget *parent = 0);
    QSize sizeHint() const;

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
    QPoint position, cursorPos;
    int keyboardRadius;
};

#endif