#ifndef KEYBOARD_WIDGET_H
#define KEYBOARD_WIDGET_H

#include "DraggableWidget.h"

#define KEYBOARD_RADIUS 200
#define RING_WIDTH 40

// TODO: Replace these with actual back-end data type.
struct keyData
{
    char main, hold;
};

struct wheelData
{
    std::vector<keyData> innerRing;
    std::vector<keyData> outerRing;
};

class KeyboardWidget : public DraggableWidget
{
    Q_OBJECT

public:
    KeyboardWidget(int radius = KEYBOARD_RADIUS, int ringWidth = RING_WIDTH, QWidget *parent = 0);
    QSize sizeHint() const;

    // TODO: Add slots for the operations.
    void addWheel(wheelData wheel);
    void clearWheels();

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
    void drawRing(QPainter &painter, std::vector<keyData> ring);

    QPoint position, cursorPos;
    std::vector<wheelData> wheels;
    int keyboardRadius, ringWidth;
    int selectedWheel;
    int selectedKey;
};

#endif