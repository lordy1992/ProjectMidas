#include <cstdlib>
#include "DistanceWidget.h"
#include "qboxlayout.h"
#include "qlabel.h"


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
    distance = (float)(db - BASE_DB)/DB_PER_M;
    distanceLabel->setText(QString::number(distance) + "m");
}

