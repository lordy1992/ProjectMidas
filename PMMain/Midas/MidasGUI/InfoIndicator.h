#ifndef INFO_INDICATOR_H
#define INFO_INDICATOR_H

#include "DraggableWidget.h"
#include "MidasCommon.h"
#include <QGridLayout.h>
#include <qpushbutton.h>

class InfoIndicator : public DraggableWidget
{
    Q_OBJECT

public:
    InfoIndicator(int widgetWidth = INFO_INDICATOR_WIDTH, int widgetHeight = INFO_INDICATOR_HEIGHT, QWidget *parent = 0);
    QSize sizeHint() const;
    

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    
private:
    QString getShowAllString();

    QPoint position;
    int indWidth, indHeight;
    QGridLayout *layout;
    QLabel *stateLabel;

    QPushButton *button;
    bool showAll;

    signals:
    void emitShowAllToggle(bool);

    private slots:
    void handleButton();

    public slots:
    void handleUpdateState(QString stateLabel);
};

#endif MOUSE_INDICATOR_H