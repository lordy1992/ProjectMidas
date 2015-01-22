#include "midasgui.h"
#include "MidasThread.h"
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SequenceDisplayer sequenceDisplayer;

    MidasThread midasThread(&sequenceDisplayer);
    midasThread.start();

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");
    qRegisterMetaType<std::vector<sequenceProgressData>>("std::vector<sequenceProgressData>");

    //MidasGUI w(&midasThread);
    MouseIndicator w1(&midasThread);

    sequenceDisplayer.show();
    w1.show();
    return a.exec();
}
