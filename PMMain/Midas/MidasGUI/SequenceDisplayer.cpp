#include "SequenceDisplayer.h"
#include <QImage.h>
#include <qevent.h>

SequenceDisplayer::SequenceDisplayer(QWidget *parent)
    : DraggableWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
    gridLayout = new QGridLayout;
    setLayout(gridLayout);

    setAttribute(Qt::WA_TranslucentBackground);
    //setWindowOpacity(0.9);
    // Test code
    QImage image1(tr("tester1.bmp"));
    QImage image2(tr("tester2.bmp"));
    QPixmap pixmap1 = QPixmap::fromImage(image1);
    QPixmap pixmap2 = QPixmap::fromImage(image2);

    std::vector<sequenceImageSet> sequence1, sequence2, sequence3;

    sequenceImageSet sequence1Step1;
    sequence1Step1.nextImage = pixmap1;
    sequence1Step1.laterImage = pixmap2;
    sequence1Step1.actionTag = 1;
    sequenceImageSet sequence1Step2;
    sequence1Step2.nextImage = pixmap1;
    sequence1Step2.laterImage = pixmap2;
    sequence1Step2.actionTag = 2;
    sequenceImageSet sequence1Step3;
    sequence1Step3.nextImage = pixmap1;
    sequence1Step3.laterImage = pixmap2;
    sequence1Step3.actionTag = 3;
    sequenceImageSet sequence1Step4;
    sequence1Step4.nextImage = pixmap1;
    sequence1Step4.laterImage = pixmap2;
    sequence1Step4.actionTag = 4;

    sequence1.push_back(sequence1Step1);
    sequence1.push_back(sequence1Step2);
    sequence1.push_back(sequence1Step3);
    sequence1.push_back(sequence1Step4);

    sequenceImageSet sequence2Step1;
    sequence2Step1.nextImage = pixmap1;
    sequence2Step1.laterImage = pixmap2;
    sequence2Step1.actionTag = 3;

    sequence2.push_back(sequence2Step1);

    sequenceImageSet sequence3Step1;
    sequence3Step1.nextImage = pixmap1;
    sequence3Step1.laterImage = pixmap2;
    sequence3Step1.actionTag = 1;
    sequenceImageSet sequence3Step2;
    sequence3Step2.nextImage = pixmap1;
    sequence3Step2.laterImage = pixmap2;
    sequence3Step2.actionTag = 2;
    sequenceImageSet sequence3Step3;
    sequence3Step3.nextImage = pixmap1;
    sequence3Step3.laterImage = pixmap2;
    sequence3Step3.actionTag = 4;

    sequence3.push_back(sequence3Step1);
    sequence3.push_back(sequence3Step2);
    sequence3.push_back(sequence3Step3);

    addSequence("Sequence 1", sequence1);
    addSequence("Sequence 2", sequence2);
    addSequence("Sequence 3", sequence3);

    addSequenceWidgets();
}

void SequenceDisplayer::addSequence(std::string sequenceName, std::vector<sequenceImageSet> sequenceImages)
{
    sequenceData newSequence;
    newSequence.sequenceImages = sequenceImages;
    newSequence.currentPos = 0;
    newSequence.numSteps = newSequence.sequenceImages.size();
    newSequence.seqLabel = new QLabel(tr("%1").arg(QString(sequenceName.c_str())));
    formBoxLabel(newSequence.seqLabel);
    newSequence.seqPosLabel = new QLabel(tr("0 / %1").arg(newSequence.numSteps));
    formBoxLabel(newSequence.seqPosLabel);

    std::vector<sequenceImageSet>::iterator it;
    for (it = newSequence.sequenceImages.begin(); it != newSequence.sequenceImages.end(); it++)
    {
        it->currentImgLabel = new QLabel;
        formBoxLabel(it->currentImgLabel);
    }

    sequenceNameToDataMap[sequenceName] = newSequence;
}

void SequenceDisplayer::advanceSequences(int action)
{
    std::map<std::string, sequenceData>::iterator it = sequenceNameToDataMap.begin();

    bool done = false;
    int rownum = 0;
    while (it != sequenceNameToDataMap.end())
    {
        sequenceImageSet images = it->second.sequenceImages.at(it->second.currentPos);
        if (images.actionTag == action)
        {
            // This sequence has a match.
            it->second.currentPos++;
            if (it->second.currentPos >= it->second.sequenceImages.size())
            {
                // Done
                done = true;
            }
            else
            {
                it->second.seqPosLabel->setText(tr("%1 / %2").arg(QString::number(it->second.currentPos), 
                    QString::number(it->second.numSteps)));
            }

            it++;
        }
        else
        {
            clearRow(it->second);
            it = sequenceNameToDataMap.erase(it);
        }
    }
    
    clearWidgets();
    if (done) {
        sequenceNameToDataMap.clear();
    }

    addSequenceWidgets();
}

void SequenceDisplayer::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_0)
    {
        advanceSequences(0);
    }
    else if (e->key() == Qt::Key_1)
    {
        advanceSequences(1);
    }
    else if (e->key() == Qt::Key_2)
    {
        advanceSequences(2);
    }
    else if (e->key() == Qt::Key_3)
    {
        advanceSequences(3);
    }
    else if (e->key() == Qt::Key_4)
    {
        advanceSequences(4);
    }
    else if (e->key() == Qt::Key_5)
    {
        advanceSequences(5);
    }
}

void SequenceDisplayer::clearRow(sequenceData seq)
{
    seq.seqLabel->setHidden(true);
    gridLayout->removeWidget(seq.seqLabel);
    seq.seqPosLabel->setHidden(true);
    gridLayout->removeWidget(seq.seqPosLabel);

    std::vector<sequenceImageSet> sequenceImages = seq.sequenceImages;
    std::vector<sequenceImageSet>::iterator sequenceIt;
    for (sequenceIt = sequenceImages.begin();
        sequenceIt != sequenceImages.end(); sequenceIt++)
    {
        sequenceIt->currentImgLabel->setHidden(true);
        gridLayout->removeWidget(sequenceIt->currentImgLabel);
    }
}

void SequenceDisplayer::clearWidgets()
{
    std::map<std::string, sequenceData>::iterator it;
    for (it = sequenceNameToDataMap.begin(); it != sequenceNameToDataMap.end(); it++)
    {
        sequenceData seq = it->second;
        clearRow(seq);
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
        seq.seqLabel->setHidden(false);
        seq.seqPosLabel->setHidden(false);
        gridLayout->addWidget(seq.seqLabel, currRow, currCol++);
        gridLayout->addWidget(seq.seqPosLabel, currRow, currCol++);
        std::vector<sequenceImageSet>::iterator sequenceIt;
        for (sequenceIt = seq.sequenceImages.begin() + seq.currentPos; 
             sequenceIt != seq.sequenceImages.end() && currCol < 5; sequenceIt++)
        {
            QPixmap pixmap = sequenceIt->laterImage;
            if (currCol == 2) pixmap = sequenceIt->nextImage;

            sequenceIt->currentImgLabel->setPixmap(pixmap);
            sequenceIt->currentImgLabel->setEnabled(!pixmap.isNull());
            sequenceIt->currentImgLabel->setHidden(false);
            gridLayout->addWidget(sequenceIt->currentImgLabel, currRow, currCol);
            currCol++;
        }

        currRow++;
    }
}