#include "midasgui.h"
#include "MidasThread.h"
#include "MouseCtrl.h"
#include "MainGUI.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MidasThread midasThread;

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");
    qRegisterMetaType<std::vector<sequenceProgressData> >("std::vector<sequenceProgressData>");

    MainGUI mainDisplay(&midasThread, MOVE_RATE_DEADZONE);

    midasThread.setDisplayHandles(mainDisplay.getSequenceDisplayer(), mainDisplay.getInfoIndicator());
    midasThread.start();

    mainDisplay.show();
    return a.exec();
}
