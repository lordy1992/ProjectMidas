#ifndef INFO_INDICATOR_H
#define INFO_INDICATOR_H

#include "DraggableWidget.h"
#include "MidasCommon.h"

class InfoIndicator : public DraggableWidget
{
    Q_OBJECT

public:
    InfoIndicator(int widgetWidth = MOUSE_INDICATOR_SIZE, int widgetHeight = INFO_INDICATOR_HEIGHT, QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QPoint position;
    int indWidth, indHeight;
    QLabel *stateLabel;

    signals:
    void emitShowAllToggle(bool);

    public slots:
    void handleUpdateState(QString stateLabel);
};

#endif MOUSE_INDICATOR_H