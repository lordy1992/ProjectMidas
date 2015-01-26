#include "midasgui.h"
#include "MidasThread.h"
#include "MouseCtrl.h"
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SequenceDisplayer sequenceDisplayer;
    InfoIndicator infoIndicator;

    MidasThread midasThread(&sequenceDisplayer, &infoIndicator);
    midasThread.start();

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");
    qRegisterMetaType<std::vector<sequenceProgressData> >("std::vector<sequenceProgressData>");

    //MidasGUI w(&midasThread);
    MouseIndicator w1(&midasThread, MOVE_RATE_DEADZONE);

    sequenceDisplayer.show();
    infoIndicator.show();
    w1.show();
    return a.exec();
}
