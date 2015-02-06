#include "KeyboardWidget.h"
#include <QEvent.h>
#include <QPainter.h>

#define _USE_MATH_DEFINES
#include <math.h>

KeyboardWidget::KeyboardWidget(int radius, int ringWidth, QWidget *parent)
    : DraggableWidget(parent), keyboardRadius(radius), ringWidth(ringWidth), selectedWheel(-1),
    selectedKey(-1)
{
    setWindowOpacity(0.75);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);

    setFixedSize(radius * 2, radius * 2);

    // Temporary Test:
    selectedWheel = 0;
    selectedKey = 0;
    wheelData wheel1;
    wheel1.outerRing.push_back(keyData('a', 'A'));
    wheel1.outerRing.push_back(keyData('b', 'B'));
    wheel1.outerRing.push_back(keyData('c', 'C'));
    wheel1.outerRing.push_back(keyData('d', 'D'));
    wheel1.outerRing.push_back(keyData('e', 'E'));
    wheel1.outerRing.push_back(keyData('f', 'F'));
    wheel1.outerRing.push_back(keyData('g', 'G'));
    wheel1.outerRing.push_back(keyData('h', 'H'));
    wheel1.outerRing.push_back(keyData('i', 'I'));
    wheel1.outerRing.push_back(keyData('j', 'J'));
    wheel1.outerRing.push_back(keyData('k', 'K'));
    wheel1.outerRing.push_back(keyData('l', 'L'));
    wheel1.outerRing.push_back(keyData('m', 'M'));

    wheel1.innerRing.push_back(keyData('n', 'N'));
    wheel1.innerRing.push_back(keyData('o', 'O'));
    wheel1.innerRing.push_back(keyData('p', 'P'));
    wheel1.innerRing.push_back(keyData('q', 'Q'));
    wheel1.innerRing.push_back(keyData('r', 'R'));
    wheel1.innerRing.push_back(keyData('s', 'S'));
    wheel1.innerRing.push_back(keyData('t', 'T'));
    wheel1.innerRing.push_back(keyData('u', 'U'));
    wheel1.innerRing.push_back(keyData('v', 'V'));
    wheel1.innerRing.push_back(keyData('w', 'W'));
    wheel1.innerRing.push_back(keyData('x', 'X'));
    wheel1.innerRing.push_back(keyData('y', 'Y'));
    wheel1.innerRing.push_back(keyData('z', 'Z'));

    wheels.push_back(wheel1);
}

void KeyboardWidget::addWheel(wheelData wheel)
{
    wheels.push_back(wheel);
}

void KeyboardWidget::clearWheels()
{
    wheels.clear();
}

void KeyboardWidget::paintEvent(QPaintEvent *event)
{
    int side = qMin(width(), height());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);

    int penWidth = 5;

    QColor keyboardBorderColour(139, 139, 131);
    QBrush brush(keyboardBorderColour);
    QPen pen(brush, penWidth);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    // Draw the borders.
    int keyboardBorderRad = keyboardRadius - (penWidth / 2);
    int keyboardDiam = keyboardBorderRad * 2;
    int outerRingBorderRadius = keyboardRadius - ringWidth;
    int outerRingBorderDiam = outerRingBorderRadius * 2;
    int innerRingBorderRadius = outerRingBorderRadius - ringWidth;
    int innerRingBorderDiam = innerRingBorderRadius * 2;
   // painter.drawEllipse(-keyboardBorderRad, -keyboardBorderRad, keyboardDiam, keyboardDiam);
   // painter.drawEllipse(-outerRingBorderRadius, -outerRingBorderRadius, outerRingBorderDiam, outerRingBorderDiam);
   // painter.drawEllipse(-innerRingBorderRadius, -innerRingBorderRadius, innerRingBorderDiam, innerRingBorderDiam);

    // Draw the key squares.
    if (selectedWheel >= 0)
    {
        std::vector<keyData> innerRing, outerRing;
        innerRing = wheels[selectedWheel].innerRing;
        outerRing = wheels[selectedWheel].outerRing;
        int deltaAngleInner = 360 / innerRing.size();
        int deltaAngleOuter = 360 / outerRing.size();

        drawRing(painter, outerRing, outerRingBorderRadius);
        drawRing(painter, innerRing, innerRingBorderRadius);
    }
}

void KeyboardWidget::drawRing(QPainter &painter, std::vector<keyData> ring, int ringInnerRad)
{
    qreal deltaAngle = 360.0 / ring.size();
    qreal startAngle = 90.0 + deltaAngle / 2;
    std::vector<keyData>::iterator it;

    QColor lineColour(0, 0, 0);
    QColor fillColour(139, 139, 131);
    QBrush borderBrush(lineColour);
    QPen pen(borderBrush, 5);
    QBrush fillBrush(fillColour);
    QColor textCol(0, 0, 0);
    QPen textPen(textCol);

    // Testing
    int ringOuterRad = ringInnerRad + ringWidth;
    QRectF outerRect(-ringOuterRad, -ringOuterRad, ringOuterRad * 2, ringOuterRad * 2);
    QRectF innerRect(-ringInnerRad, -ringInnerRad, ringInnerRad * 2, ringInnerRad * 2);
    for (it = ring.begin(); it != ring.end(); it++)
    {
        QPainterPath path;
        qreal startX = (ringInnerRad + ringWidth) * cos(startAngle * (M_PI / 180.0));
        qreal startY = -1 * (ringInnerRad + ringWidth) * sin(startAngle * (M_PI / 180.0));
        qreal acrossX = ringInnerRad * cos((startAngle - deltaAngle) * (M_PI / 180.0));
        qreal acrossY = -1 * ringInnerRad * sin((startAngle - deltaAngle) * (M_PI / 180.0));

        qreal textPointX = (ringInnerRad + ringWidth / 2) * cos((startAngle - deltaAngle / 2) * (M_PI / 180.0));
        qreal textPointY = -1 * (ringInnerRad + ringWidth / 2) * sin((startAngle - deltaAngle / 2) * (M_PI / 180.0));

        path.moveTo(startX, startY);
        path.arcTo(outerRect, startAngle, -deltaAngle);
        path.lineTo(acrossX, acrossY);
        path.arcTo(innerRect, startAngle - deltaAngle, deltaAngle);
        path.lineTo(startX, startY);

        painter.setPen(pen);
        painter.setBrush(fillBrush);
        painter.drawPath(path);

        QFont holdFont("Times", 9, QFont::Normal);
        QFont timesFont("Times", 16, QFont::DemiBold);
        painter.setFont(timesFont);
        painter.setPen(textPen);

        QFontMetrics fontMetrics = painter.fontMetrics();
        QString mainText = QString(it->main);
        QString holdText = QString(it->hold);

        int mainTextWidth = fontMetrics.width(mainText);

        painter.setFont(holdFont);
        int holdTextWidth = fontMetrics.width(holdText);

        painter.setFont(timesFont);
        textPointX = textPointX - mainTextWidth / 2 - holdTextWidth / 2;
        textPointY += fontMetrics.height() / 4;

        int holdPointX = textPointX + mainTextWidth + 2;

        QPoint textPoint;
        textPoint.setX(textPointX);
        textPoint.setY(textPointY);

        QPoint holdTextPoint;
        holdTextPoint.setX(holdPointX);
        holdTextPoint.setY(textPointY);

        QColor tmpCol(255, 0, 0);
        QPen tmpPen(tmpCol, 5);
        painter.setPen(tmpPen);
        painter.drawPoint(textPointX, textPointY);
        painter.setPen(textPen);

        painter.drawText(textPoint, mainText);

        painter.setFont(holdFont);
        painter.drawText(holdTextPoint, holdText);
        startAngle -= deltaAngle;
    }
}

void KeyboardWidget::resizeEvent(QResizeEvent *event)
{
    int side = qMin(width(), height());
    QRegion maskedRegion(width() / 2 - side / 2, height() / 2 - side / 2, side, side, QRegion::Ellipse);
    setMask(maskedRegion);
}

QSize KeyboardWidget::sizeHint() const
{
    return QSize(keyboardRadius, keyboardRadius);
}