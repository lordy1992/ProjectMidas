#include <cmath>
#include <cstdlib>
#include <cstring>
#include "DistanceWidget.h"
#include "qboxlayout.h"


DistanceWidget::DistanceWidget(MidasThread* mainThread, QWidget *parent, int width, int height)
    : QWidget(parent, Qt::FramelessWindowHint), distance(0), dispWidth(width), dispHeight(height)
{
    distanceLabel = new QLabel;
    QVBoxLayout *layout = new QVBoxLayout;
    QFont font("Courier", 30, QFont::Bold);

    distanceLabel->setFont(font);
    distanceLabel->setText("0m");
    layout->addWidget(distanceLabel);

    setWindowOpacity(0.75);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);

    setFixedSize(dispWidth, dispHeight);

    setLayout(layout);

    connect(mainThread, SIGNAL(emitRssi(float)), this, SLOT(updateDistanceLabel(float)));
}

void DistanceWidget::updateDistanceLabel(float db)
{
    distance = dbToDist(db);

    if (distance < 0.5)
    {
        distanceLabel->setText(QString::number(distance) + "m\nImmediate");
    }
    else if (distance < 4)
    {
        distanceLabel->setText(QString::number(distance) + "m\nNear");
    }
    else
    {
        distanceLabel->setText(QString::number(distance) + "m\nFar");
    }
}

inline float DistanceWidget::dbToDist(float db)
{
    return 3.8429*std::pow(10, -6)*std::pow(db, 4) +
        7.55606*std::pow(10, -4)*std::pow(db, 3) +
        5.34042218*std::pow(10, -2)*std::pow(db, 2) +
        1.5151387757*db +
        13.8505553761;
}