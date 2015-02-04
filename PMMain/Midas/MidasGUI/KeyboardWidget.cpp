#include "KeyboardWidget.h"
#include <QEvent.h>
#include <QPainter.h>

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
    painter.drawEllipse(-keyboardBorderRad, -keyboardBorderRad, keyboardDiam, keyboardDiam);
    painter.drawEllipse(-outerRingBorderRadius, -outerRingBorderRadius, outerRingBorderDiam, outerRingBorderDiam);
    painter.drawEllipse(-innerRingBorderRadius, -innerRingBorderRadius, innerRingBorderDiam, innerRingBorderDiam);

    // Draw the key squares.
    if (selectedWheel >= 0)
    {
        std::vector<keyData> innerRing, outerRing;
        innerRing = wheels[selectedWheel].innerRing;
        outerRing = wheels[selectedWheel].outerRing;
        int deltaAngleInner = 360 / innerRing.size();
        int deltaAngleOuter = 360 / outerRing.size();

        int startAngle = 90 + 
        QPainterPath path;
    }
}

void KeyboardWidget::drawRing(QPainter &painter, std::vector<keyData> ring)
{
    int deltaAngle = 360 / ring.size();
    int startAngle = 90 + deltaAngle / 2;
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