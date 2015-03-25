#ifndef DISTANCE_BAR_H
#define DISTANCE_BAR_H

#include <MidasCommon.h>
#include <QGraphicsRectItem>
#include <qlabel.h>
#include <qpainter.h>
#include <QPaintEvent>
#include <qrect.h>
#include <qwidget.h>

class distanceBar : public QWidget
{
    Q_OBJECT
public:
    distanceBar(qreal x, qreal y, QWidget *parent = 0, qreal width = DISTANCE_DISPLAY_WIDTH,
        qreal height = DISTANCE_DISPLAY_HEIGHT);
 
    enum class bins{
        OOR = 0,
        SIG_FAR = 1,
        SIG_NEAR = 2,
        SIG_IMMEDIATE = 3
    };

public slots:
    void setValue(int bin);

protected:
    void paintEvent(QPaintEvent *e);
        
private:
    QRect *long_bar;
    QGraphicsRectItem *med_bar;
    QGraphicsRectItem *short_bar;
    QLabel  *dist_text;
    QColor  colour;
    QString current_bin_text;
    int     current_bin;

    const qreal bar_width = 10;
    const qreal long_bar_height = 15;
    const qreal med_bar_height = 10;
    const qreal short_bar_height = 5;
    const qreal bar_margin = 2;
    const QString  *sOutOfRange = new QString("Out of Range");
    const QString  *sFar = new QString("Low Signal");
    const QString  *sNear = new QString("Medium Signal");
    const QString  *sImmediate = new QString("Strong Signal");
    const QColor OOS_colour = Qt::red;
    const QColor Far_colour = Qt::yellow;
    const QColor Near_colour = QColor(0, 128, 255);
    const QColor Immediate_colour = Qt::green;



};
#endif /* DISTANCE_BAR_H */
