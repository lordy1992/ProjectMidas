#ifndef SEQUENCE_DISPLAYER_H
#define SEQUENCE_DISPLAYER_H

#include "DraggableWidget.h"
#include "MyoCommon.h"
#include "MidasThread.h"
#include <QGridLayout.h>
#include <string>
#include <map>
#include <vector>
#include <string>

#define NUM_COLUMNS 5

Q_DECLARE_METATYPE(std::vector<sequenceImageSet>)

// A grid with 5 columns, and a certain number of rows.
class SequenceDisplayer : public DraggableWidget
{
    Q_OBJECT

public:
    SequenceDisplayer(MidasThread* midasThread, QWidget *parent = 0);

    // QPixmap uses implicit data sharing, so it is okay to pass around by value.
    void keyPressEvent(QKeyEvent *e);
    QSize sizeHint() const;

public slots:
    void addSequence(std::string sequenceName, std::vector<sequenceImageSet> sequenceImages);
    void advanceSequences(int action);
    void updateSequences();
 
private:
    void clearWidgets(bool deleteLabels = false);
    void clearRow(sequenceData seq, bool deleteLabels = false);
    void formBoxLabel(QLabel *label);

    std::map<std::string, sequenceData> sequenceNameToDataMap;
    QGridLayout *gridLayout;
    int maxWidth, maxHeight;
    int maxNumSequences;
    MidasThread* midasThread;
};

#endif SEQUENCE_DISPLAYER_H