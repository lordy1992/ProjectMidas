#include "KeyboardWidget.h"
#include <QEvent.h>
#include <QPainter.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define POPOUT_DISTANCE 10
#define EXTRA_WINDOW_DIST 10

KeyboardWidget::KeyboardWidget(MidasThread *mainThread, int radius, int ringWidth, QWidget *parent)
    : DraggableWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint), 
        keyboardRadius(radius), ringWidth(ringWidth), selectedWheel(-1),
    selectedKey(-1)
{
   // setWindowOpacity(0.75);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);
    //QPalette pal;
    //pal.setColor(QPalette::Background, QColor(205, 205, 193));
   // setAutoFillBackground(true);
   // setPalette(pal);

    setFixedSize(radius * 2 + POPOUT_DISTANCE + EXTRA_WINDOW_DIST, radius * 2 + POPOUT_DISTANCE + EXTRA_WINDOW_DIST);

    // Temporary Test:
    selectedWheel = 0;
    //selectedKey = 5;
    //outerSelected = true;
 
    connect(mainThread, SIGNAL(emitUpdateKeyboard(int, double, bool, bool)), this, SLOT(updateKeyboard(int, double, bool, bool)));
    connect(mainThread, SIGNAL(emitKeyboardData(int, double)), this, SLOT(handleKeyboardData(int, double)));

    tempDebugText1 = "text1";
    tempDebugText2 = "text2";
    tempDebugText3 = "text3";
    tempDebugText4 = "text4";
}

void KeyboardWidget::addWheel(ringData wheel)
{
    wheels.push_back(wheel);
}

void KeyboardWidget::addWheels(std::vector<ringData> *kybrdRingData)
{
    for (int i = 0; i < kybrdRingData->size(); i++)
    {
        addWheel((*kybrdRingData)[i]);
    }
}

void KeyboardWidget::clearWheels()
{
    wheels.clear();
}

void KeyboardWidget::updateKeyboard(int wheelNumber, double currAngle, bool center, bool held)
{
    selectedWheel = wheelNumber / 2;
    outerSelected = (wheelNumber % 2 == 0);
    selectedKey = getSelectedKeyFromAngle(currAngle);

    update();
}

void KeyboardWidget::handleKeyboardData(int wheelNumber, double currAngle)
{
    selectedWheel = wheelNumber / 2;
    outerSelected = (wheelNumber % 2 == 0);
    selectedKey = getSelectedKeyFromAngle(currAngle);

    tempDebugText1 = std::to_string(currAngle).c_str();

    update();
}

int KeyboardWidget::getSelectedKeyFromAngle(double angle)
{
    std::vector<ringData::keyboardValue> *ring;

    if (outerSelected)
    {
        ring = wheels[selectedWheel].getRingOutVectorHandle();
    }
    else
    {
        ring = wheels[selectedWheel].getRingInVectorHandle();
    }

    qreal deltaAngle = 360.0 / ring->size();
    //qreal startAngle = 90.0 + deltaAngle / 2; // TODO - normalize on backend side.

    // TODO: May have to change later, based on received angle
    return (int)((angle) / deltaAngle);
}

void KeyboardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);

    int penWidth = 5;

    QColor keyboardBorderColour(0, 0, 100);
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
    //painter.drawEllipse(-keyboardBorderRad, -keyboardBorderRad, keyboardDiam, keyboardDiam);
    //painter.drawEllipse(-outerRingBorderRadius, -outerRingBorderRadius, outerRingBorderDiam, outerRingBorderDiam);
    QBrush fillCenterBrush(QColor(205, 205, 193));
    painter.setBrush(fillCenterBrush);
    painter.drawEllipse(-innerRingBorderRadius, -innerRingBorderRadius, innerRingBorderDiam, innerRingBorderDiam);

    QString temp;
    painter.drawText(QPoint(0, 30), tempDebugText1);
    painter.drawText(QPoint(0, 10), tempDebugText2);
    painter.drawText(QPoint(0, -10), tempDebugText3);
    painter.drawText(QPoint(0, -30), tempDebugText4);

    // Draw the key squares.
    if (selectedWheel >= 0)
    {
        std::vector<ringData::keyboardValue> *innerRing, *outerRing;
        innerRing = wheels[selectedWheel].getRingInVectorHandle();
        outerRing = wheels[selectedWheel].getRingOutVectorHandle();
        int deltaAngleInner = 360 / innerRing->size();
        int deltaAngleOuter = 360 / outerRing->size();

        if (outerSelected)
        {
            drawRing(painter, innerRing, innerRingBorderRadius - 2, !outerSelected);
            drawRing(painter, outerRing, outerRingBorderRadius - 2, outerSelected);
        }
        else
        {
            drawRing(painter, outerRing, outerRingBorderRadius - 2, outerSelected);
            drawRing(painter, innerRing, innerRingBorderRadius - 2, !outerSelected);
        }
    }
}

void KeyboardWidget::drawRing(QPainter &painter, std::vector<ringData::keyboardValue> *ring, int ringInnerRad, bool isSelected)
{
    qreal deltaAngle = 360.0 / ring->size();
    qreal startAngle = 90.0 + deltaAngle / 2;
    std::vector<ringData::keyboardValue>::iterator it;

    int ringOuterRad = ringInnerRad + ringWidth;
    QRectF outerRect(-ringOuterRad, -ringOuterRad, ringOuterRad * 2, ringOuterRad * 2);
    QRectF innerRect(-ringInnerRad, -ringInnerRad, ringInnerRad * 2, ringInnerRad * 2);
    int currentKey = 0;
    qreal selectedAngle = 0.0;
    bool foundSelected = false;
    QColor lineColour(0, 0, 100);
    QColor lineColourSelected(200, 0, 0);
    for (it = ring->begin(); it != ring->end(); it++)
    {
        if (isSelected && selectedKey == currentKey)
        {
            foundSelected = true;
            selectedAngle = startAngle;
        }
        else
        {
            drawKey(painter, ringInnerRad, startAngle, deltaAngle, outerRect, 
                innerRect, *it, lineColour, ringWidth);
        }

        startAngle -= deltaAngle;
        currentKey++;
    }

    if (foundSelected)
    {
        // Draw the selected.
        int selectRad = ringOuterRad + POPOUT_DISTANCE;
        QRectF selectRect(-selectRad, -selectRad, selectRad * 2, selectRad * 2);
        drawKey(painter, ringInnerRad, selectedAngle, deltaAngle, selectRect, innerRect, (*ring)[selectedKey],
            lineColourSelected, ringWidth + POPOUT_DISTANCE);
    }
}

void KeyboardWidget::drawKey(QPainter &painter, int ringInnerRad, qreal currAngle, qreal deltaAngle, 
    QRectF& outerRect, QRectF& innerRect, ringData::keyboardValue keyDat, QColor& lineColour, int distBetween)
{
    QColor fillColour(160, 182, 215);
    QBrush borderBrush(lineColour);
    QPen pen(borderBrush, 5);
    QBrush fillBrush(fillColour);
    QColor textCol(0, 0, 0);
    QPen textPen(textCol);

    QPainterPath path;
    qreal startX = (ringInnerRad + distBetween) * cos(currAngle * (M_PI / 180.0));
    qreal startY = -1 * (ringInnerRad + distBetween) * sin(currAngle * (M_PI / 180.0));
    qreal acrossX = ringInnerRad * cos((currAngle - deltaAngle) * (M_PI / 180.0));
    qreal acrossY = -1 * ringInnerRad * sin((currAngle - deltaAngle) * (M_PI / 180.0));

    qreal textPointX = (ringInnerRad + distBetween / 2) * cos((currAngle - deltaAngle / 2) * (M_PI / 180.0));
    qreal textPointY = -1 * (ringInnerRad + distBetween / 2) * sin((currAngle - deltaAngle / 2) * (M_PI / 180.0));

    path.moveTo(startX, startY);
    path.arcTo(outerRect, currAngle, -deltaAngle);
    path.lineTo(acrossX, acrossY);
    path.arcTo(innerRect, currAngle - deltaAngle, deltaAngle);
    path.lineTo(startX, startY);

    painter.setPen(pen);
    painter.setBrush(fillBrush);
    painter.drawPath(path);

    QFont holdFont("Times", 9, QFont::Normal);
    QFont timesFont("Times", 16, QFont::DemiBold);
    painter.setFont(timesFont);
    painter.setPen(textPen);

    QFontMetrics fontMetrics = painter.fontMetrics();
    QString mainText = QString(keyDat.main);
    QString holdText = QString(keyDat.hold);

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

    painter.drawText(textPoint, mainText);

    painter.setFont(holdFont);
    painter.drawText(holdTextPoint, holdText);
}

void KeyboardWidget::resizeEvent(QResizeEvent *event)
{
    //int side = qMin(width(), height());
    //QRegion maskedRegion(width() / 2 - side / 2, height() / 2 - side / 2, side, side, QRegion::Ellipse);
    //setMask(maskedRegion);
}

QSize KeyboardWidget::sizeHint() const
{
    return QSize(keyboardRadius * 2 + POPOUT_DISTANCE + EXTRA_WINDOW_DIST, keyboardRadius * 2 + POPOUT_DISTANCE + EXTRA_WINDOW_DIST);
}