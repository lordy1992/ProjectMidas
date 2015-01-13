#include "SequenceDisplayer.h"

SequenceDisplayer::SequenceDisplayer(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
    gridLayout = new QGridLayout;
    setLayout(gridLayout);
}

void SequenceDisplayer::addSequence(std::string sequenceName, std::vector<sequenceImageSet> sequenceImages)
{
    sequenceData newSequence;
    newSequence.currentPos = sequenceImages.begin();
    newSequence.numSteps = sequenceImages.size();
    newSequence.seqLabel = new QLabel(tr("%1").arg(QString(sequenceName.c_str())));
    formBoxLabel(newSequence.seqLabel);
    newSequence.seqPosLabel = new QLabel(tr("0 / %1").arg(newSequence.numSteps));
    formBoxLabel(newSequence.seqPosLabel);

    std::vector<sequenceImageSet>::iterator it;
    for (it = sequenceImages.begin(); it != sequenceImages.end(); it++)
    {
        it->currentImgLabel = new QLabel;
        formBoxLabel(it->currentImgLabel);
    }
}

void SequenceDisplayer::formBoxLabel(QLabel *label)
{
    label->setEnabled(false);
    label->setFrameShape(QFrame::Box);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setBackgroundRole(QPalette::Base);
    label->setAlignment(Qt::AlignCenter);
    label->setAutoFillBackground(true);
    label->setMinimumSize(96, 96);
}

void SequenceDisplayer::addSequenceWidgets()
{
    std::map<std::string, sequenceData>::iterator it;

    int currRow = 0;
    for (it = sequenceNameToDataMap.begin(); it != sequenceNameToDataMap.end(); it++)
    {
        sequenceData seq = it->second;
        int currCol = 0;
        gridLayout->addWidget(seq.seqLabel, currRow, currCol++);
        gridLayout->addWidget(seq.seqPosLabel, currRow, currCol++);
        std::vector<sequenceImageSet>::iterator sequenceIt;
        for (sequenceIt = seq.currentPos; 
             sequenceIt != seq.sequenceImages.end() && currCol < 5; sequenceIt++)
        {
            QPixmap pixmap = sequenceIt->laterImage;
            if (currCol == 2) pixmap = sequenceIt->nextImage;

            sequenceIt->currentImgLabel->setPixmap(pixmap);
            sequenceIt->currentImgLabel->setEnabled(!pixmap.isNull());
            gridLayout->addWidget(sequenceIt->currentImgLabel, currRow, currCol);
            currCol++;
        }

        currRow++;
    }
}