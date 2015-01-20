#include "SequenceDisplayer.h"
#include <QImage.h>
#include <qevent.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

// Temporary 
#include "SequenceImageManager.h"

#define LABEL_NUM_COLS      3
#define LABEL_NUM_ROWS      1
#define SEQ_NUMBER_NUM_COLS 1
#define SEQ_NUMBER_NUM_ROWS 1
#define NUM_SEQUENCE_STEPS  3
#define GRID_ELEMENT_SIZE   48
#define NUM_COLS (LABEL_NUM_COLS + SEQ_NUMBER_NUM_COLS + NUM_SEQUENCE_STEPS)
#define GUI_WIDTH_BUFFER 1
#define GUI_HEIGHT_OFFSET_FROM_BOTTOM 96

SequenceDisplayer::SequenceDisplayer(MidasThread* midasThread, QWidget *parent)
    : DraggableWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
    this->midasThread = midasThread;

    gridLayout = new QGridLayout;
    gridLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    gridLayout->setSpacing(5);
    setLayout(gridLayout);
    maxNumSequences = 10;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

    maxHeight = GRID_ELEMENT_SIZE * maxNumSequences;
    maxWidth = GRID_ELEMENT_SIZE * (NUM_COLS + GUI_WIDTH_BUFFER);

    // Position the widget on the bottom-right initially.
    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - maxWidth, screen.bottom() - maxHeight - GUI_HEIGHT_OFFSET_FROM_BOTTOM, 
        maxWidth, maxHeight);

    connect(midasThread, SIGNAL(emitAddSequence(std::string, std::vector<sequenceImageSet>)), this, SLOT(addSequence(std::string, std::vector<sequenceImageSet>)));
    connect(midasThread, SIGNAL(emitAdvanceSequence(int)), this, SLOT(advanceSequences(int)));
    connect(midasThread, SIGNAL(emitUpdateSequences()), this, SLOT(updateSequences()));

    // Test code
    SequenceImageManager imgManager;
    imgManager.loadImages();

    std::vector<sequenceImageSet> sequence1, sequence2, sequence3, sequence4;
    std::vector<int> ids1 = { 1, 2, 3, 2 };
    std::vector<int> ids2 = { 1, 2, 4, 6 };
    std::vector<int> ids3 = { 1, 3, 2 };
    std::vector<int> ids4 = { 2, 6 };
    sequence1 = imgManager.formSequenceSetFromIds(ids1);
    sequence2 = imgManager.formSequenceSetFromIds(ids2);
    sequence3 = imgManager.formSequenceSetFromIds(ids3);
    sequence4 = imgManager.formSequenceSetFromIds(ids4);

    addSequence("Zoom In", sequence1);
    addSequence("Rotate Screen", sequence2);
    addSequence("Wiggle", sequence3);
    addSequence("Double View", sequence4);

    updateSequences();
}

QSize SequenceDisplayer::sizeHint() const
{
    return QSize(maxWidth, maxHeight);
}

void SequenceDisplayer::addSequence(std::string sequenceName, std::vector<sequenceImageSet> sequenceImages)
{
    sequenceData newSequence;
    newSequence.sequenceImages = sequenceImages;
    newSequence.currentPos = 0;
    newSequence.numSteps = newSequence.sequenceImages.size();
    QFont timesFont("Times", 9, QFont::Bold);
    newSequence.seqLabel = new QLabel(tr("%1").arg(QString(sequenceName.c_str())));
    newSequence.seqLabel->setFont(timesFont);
    newSequence.seqLabel->setWordWrap(true);
    formBoxLabel(newSequence.seqLabel);
    newSequence.seqLabel->setFixedSize(GRID_ELEMENT_SIZE * LABEL_NUM_COLS, GRID_ELEMENT_SIZE);
    newSequence.seqPosLabel = new QLabel(tr("0 / %1").arg(newSequence.numSteps));
    newSequence.seqPosLabel->setFont(timesFont);
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
            clearRow(it->second, true);
            it = sequenceNameToDataMap.erase(it);
        }
    }
    
    if (done) {
        clearWidgets(true);
        sequenceNameToDataMap.clear();
    } else {
        clearWidgets();
    }

    updateSequences();
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

void SequenceDisplayer::clearRow(sequenceData seq, bool deleteLabels)
{
    seq.seqLabel->setHidden(true);
    gridLayout->removeWidget(seq.seqLabel);
    seq.seqPosLabel->setHidden(true);
    gridLayout->removeWidget(seq.seqPosLabel);

    if (deleteLabels)
    {
        delete seq.seqLabel;
        delete seq.seqPosLabel;
        seq.seqLabel = seq.seqPosLabel = NULL;
    }

    std::vector<sequenceImageSet> sequenceImages = seq.sequenceImages;
    std::vector<sequenceImageSet>::iterator sequenceIt;
    for (sequenceIt = sequenceImages.begin();
        sequenceIt != sequenceImages.end(); sequenceIt++)
    {
        sequenceIt->currentImgLabel->setHidden(true);
        gridLayout->removeWidget(sequenceIt->currentImgLabel);
        if (deleteLabels)
        {
            delete sequenceIt->currentImgLabel;
            sequenceIt->currentImgLabel = NULL;
        }
    }
}

void SequenceDisplayer::clearWidgets(bool deleteLabels)
{
    std::map<std::string, sequenceData>::iterator it;
    for (it = sequenceNameToDataMap.begin(); it != sequenceNameToDataMap.end(); it++)
    {
        sequenceData seq = it->second;
        clearRow(seq, deleteLabels);
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
    label->setFixedSize(GRID_ELEMENT_SIZE, GRID_ELEMENT_SIZE);
    label->setScaledContents(true);
}

void SequenceDisplayer::updateSequences()
{
    std::map<std::string, sequenceData>::iterator it;

    int currRow = 0;
    for (it = sequenceNameToDataMap.begin(); it != sequenceNameToDataMap.end(); it++)
    {
        sequenceData seq = it->second;
        int currCol = 0;
        seq.seqLabel->setHidden(false);
        seq.seqPosLabel->setHidden(false);
        gridLayout->addWidget(seq.seqLabel, currRow, currCol, LABEL_NUM_ROWS, LABEL_NUM_COLS);
        currCol += LABEL_NUM_COLS;
        gridLayout->addWidget(seq.seqPosLabel, currRow, currCol, SEQ_NUMBER_NUM_ROWS, SEQ_NUMBER_NUM_COLS);
        currCol += SEQ_NUMBER_NUM_COLS;
        std::vector<sequenceImageSet>::iterator sequenceIt;
        for (sequenceIt = seq.sequenceImages.begin() + seq.currentPos; 
            sequenceIt != seq.sequenceImages.end() && currCol < NUM_COLS; sequenceIt++)
        {
            QPixmap pixmap = sequenceIt->laterImage;
            if (currCol == LABEL_NUM_COLS + SEQ_NUMBER_NUM_COLS) pixmap = sequenceIt->nextImage;

            sequenceIt->currentImgLabel->setPixmap(pixmap);
            sequenceIt->currentImgLabel->setEnabled(!pixmap.isNull());
            sequenceIt->currentImgLabel->setHidden(false);
            gridLayout->addWidget(sequenceIt->currentImgLabel, currRow, currCol);
            currCol++;
        }

        currRow++;
    }
}