#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include <QDialog>
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"

class MainGUI : public DraggableWidget
{
    Q_OBJECT

public:
    MainGUI(MidasThread *mainThread, int deadZoneRad);
    ~MainGUI();

    // Temporary
    MouseIndicator *getMouseIndicator();
    InfoIndicator *getInfoIndicator();
    SequenceDisplayer *getSequenceDisplayer();

private:
    QVBoxLayout *layout;
    MouseIndicator *mouseIndicator;
    InfoIndicator *infoIndicator;
    SequenceDisplayer *sequenceDisplayer;
};

#endif