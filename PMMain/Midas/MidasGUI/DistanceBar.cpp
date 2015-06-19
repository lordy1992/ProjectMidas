#ifdef BUILD_KEYBOARD_ANDDISTANCE
#include "DistanceBar.h"

distanceBar::distanceBar(qreal x, qreal y, QWidget *parent, qreal width,
    qreal height) : parent(parent)
{
    //long_bar = new QRect(x, y, bar_width, long_bar_height);
    long_bar = new QRect(x, y, width, height);
    colour = OOS_colour;
    current_bin_text = *sOutOfRange;
}

void distanceBar::setValue(int bin)
{
    if (current_bin != bin)
    {
        switch (bin)
        {
        case (int)bins::SIG_FAR:
            colour = Far_colour;
            current_bin_text = *sFar;
            break;
        case (int)bins::SIG_NEAR:
            colour = Near_colour;
            current_bin_text = *sNear;
            break;
        case (int)bins::SIG_IMMEDIATE:
            colour = Immediate_colour;
            current_bin_text = *sImmediate;
            break;
        default:
            colour = OOS_colour;
            current_bin_text = *sOutOfRange;
            break;
        }
        parent->update();
        current_bin = bin;
    }
}

void distanceBar::paintEvent(QPaintEvent *e)
{
    QRect rect = e->rect();
    QPainter painter(this);
    QBrush brush(colour, Qt::SolidPattern);
    QFont font("Times", 9, QFont::Bold);
    
    painter.setBrush(brush);
    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawRect(rect);
    painter.drawText(rect, Qt::AlignCenter, current_bin_text);
}


#endif