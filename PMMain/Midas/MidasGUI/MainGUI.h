#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include <QMainWindow>
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"

class MainGUI : public QMainWindow
{
    Q_OBJECT

public:
    MainGUI(MidasThread *mainThread, int deadZoneRad);

private:
    MouseIndicator mouseIndicator;
    InfoIndicator infoIndicator;
    SequenceDisplayer sequenceDisplayer;

};

#endif