#ifndef SEQUENCE_DISPLAYER_H
#define SEQUENCE_DISPLAYER_H

#include "DraggableWidget.h"
#include "MyoCommon.h"
#include <QGridLayout.h>
#include <string>
#include <map>
#include <vector>
#include <string>

#define NUM_COLUMNS 5

Q_DECLARE_METATYPE(std::vector<sequenceImageSet>)
Q_DECLARE_METATYPE(std::vector<sequenceProgressData>)

/**
 * The SequenceDisplayer class represents a GUI that can display Midas gesture sequences. These gesture sequences
 * are made up of a series of images that should convey information to the user on what gesture they must perform
 * next in the sequence.
 */
class SequenceDisplayer : public DraggableWidget
{
    Q_OBJECT

public:
    /**
     * Constructor for the SequenceDisplayer.
     *
     * @param parent The parent widget.
     */
    SequenceDisplayer(QWidget *parent = 0);

    /**
     * Returns the recommended size of the widget.
     *
     * @return The recommended size of the widget.
     */
    QSize sizeHint() const;

public slots:
    /**
     * Register a sequence, with an ID, name, and set of images.
     *
     * @param id The id of the sequence.
     * @param name A human-readable name for the sequence.
     * @param images The images that make up the sequence. Each element of the vector contains two images.
     */
    void registerSequenceImages(int, QString, std::vector<sequenceImageSet>);

    /**
     * Show the provided sequences on the display. The caller supplied a vector of pairs; each pair has
     * a sequence ID and a number that represents the current position in the sequence.
     *
     * @param sequenceIdProgressPairs A vector containing the pairs of sequence IDs and sequence positions.
     */
    void showSequences(std::vector<sequenceProgressData>);
 
private:
    /**
     * Clear the widgets in the grid layout.
     *
     * @param deleteLabels If true, deletes the labels in the sequences to free memory. Otherwise, only hides the 
     *        labels in the layout.
     */
    void clearWidgets(bool deleteLabels = false);

    /**
     * Clears the widgets in a row in the grid layout.
     *
     * @param seq The row of the grid layout to clear.
     * @param deleteLabels If true, deletes the labels in the row to free memory. Otherwise, only hides the labels.
     */
    void clearRow(sequenceData seq, bool deleteLabels = false);

    /**
     * Sets the properties of the provided label to conform to the look of the members of the grid layout.
     *
     * @param label The label to set properties for.
     */
    void formBoxLabel(QLabel *label);

    /**
     * Updates the GUI with the new active sequence information.
     */
    void updateSequences();

    std::map<int, sequenceData> sequenceIdToDataMap;
    std::map<int, sequenceData> activeSequencesIdToDataMap;
    QGridLayout *gridLayout;
    int maxWidth, maxHeight;
    int maxNumSequences;
};

#endif SEQUENCE_DISPLAYER_H