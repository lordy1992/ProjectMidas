#include <cmath>
#include <cstdlib>
#include <cstring>
#include "DistanceWidget.h"
#include "qboxlayout.h"


DistanceWidget::DistanceWidget(MidasThread* mainThread, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint), distance(0)
{
    distanceLabel = new QLabel;
    QVBoxLayout *layout = new QVBoxLayout;
    QFont font("Courier", 30, QFont::Bold);

    distanceLabel->setFont(font);
    distanceLabel->setText("0m");
    layout->addWidget(distanceLabel);

    setLayout(layout);

    connect(mainThread, SIGNAL(emitRssi(float)), this, SLOT(updateDistanceLabel(float)));
}

void DistanceWidget::updateDistanceLabel(float db)
{
    distance = dbToDist(db);
    //distance = (float)(db - BASE_DB)/DB_PER_M;
    distanceLabel->setText(QString::number(distance) + "m");
}

inline float DistanceWidget::dbToDist(float db)
{
    return -0.0001*std::pow(db, 3) - 0.0223*std::pow(db, 2) - 1.3376*db - 26.071;
}