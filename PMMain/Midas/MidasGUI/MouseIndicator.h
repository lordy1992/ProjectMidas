#ifndef MOUSE_INDICATOR_H
#define MOUSE_INDICATOR_H

#include "DraggableWidget.h"
#include "MidasThread.h"

/**
 * The MouseIndicator class represents a circular GUI which tracks Midas mouse velocity. The GUI supports a
 * 'dead zone' for the mouse, which looks like a smaller circle within the display. It shows a small cursor
 * that moves about that represents the current mouse velocity.
 */
class MouseIndicator : public DraggableWidget
{
    Q_OBJECT

public:
    /**
     * The constructor for the MouseIndicator. 
     *
     * @param mainThread The main thread of the Midas application; used to retrieve mouse velocity information.
     * @param deadZoneRad The radius of the dead zone, expressed as a percentage of the range of possible velocity values.
     * @param widgetWidth The width of the widget.
     * @param widgetHeight The height of the widget.
     * @param parent The parent widget.
     */
    MouseIndicator(MidasThread *mainThread, int deadZoneRad, int widgetWidth = 100, int widgetHeight = 100, QWidget *parent = 0);
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
    int indWidth, indHeight;
    int deadZoneRadius;
    MidasThread *mainThread;

public slots:
    /**
     * A slot that updates the cursor position based on the provided 'velocity percentages'. These 
     * velocity percentages are expressed as a percent of the maximum velocity, ranging
     * from -100 to 100, where -100 represents the maximum velocity in the negative direction and
     * +100 represents the maximum velocity in the positive direction.
     *
     * @param percentX A percent of the maximum x velocity, where the sign indicates direction, between -100 and 100.
     * @param percentY A percent of the maximum y velocity, where the sign indicates direction, between -100 and 100.
     */
    void handleUpdateCursorPos(int percentX, int percentY);
};

#endif MOUSE_INDICATOR_H