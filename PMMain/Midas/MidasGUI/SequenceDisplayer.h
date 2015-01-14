#ifndef SEQUENCE_DISPLAYER_H
#define SEQUENCE_DISPLAYER_H

#include "DraggableWidget.h"
#include <QGridLayout.h>
#include <string>
#include <map>
#include <vector>
#include <string>
#include <QLabel.h>

#define NUM_COLUMNS 5

/*
 * Each step in a sequence may have three different images, depending on
 * the progress of the sequence. The 'doneImage' is displayed on sequence
 * steps that have already been performed; the 'nextImage' is displayed for
 * the next step that must be performed, and the 'laterImage' applies to all
 * other steps.
 */
struct sequenceImageSet
{
    QPixmap nextImage, laterImage;
    QLabel* currentImgLabel;
};

struct sequenceData
{
    QLabel *seqLabel, *seqPosLabel;
    int numSteps, currentPos;
    std::vector<sequenceImageSet> sequenceImages;
};

// A grid with 5 columns, and a certain number of rows.
class SequenceDisplayer : public DraggableWidget
{
    Q_OBJECT

public:
    SequenceDisplayer(QWidget *parent = 0);

    // QPixmap uses implicit data sharing, so it is okay to pass around by value.
    void addSequence(std::string sequenceName, std::vector<sequenceImageSet> sequenceImages);

private:
    void addSequenceWidgets();
    void formBoxLabel(QLabel *label);
    std::map<std::string, sequenceData> sequenceNameToDataMap;
    QGridLayout *gridLayout;
};

#endif SEQUENCE_DISPLAYER_H