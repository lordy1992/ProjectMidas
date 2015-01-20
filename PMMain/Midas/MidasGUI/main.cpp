#include "midasgui.h"
#include "MidasThread.h"
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MidasThread midasThread;
    midasThread.start();

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");

    //MidasGUI w(&midasThread);
    MouseIndicator w1(&midasThread);
    SequenceDisplayer w(&midasThread);

    w1.show();
    w.show();
    return a.exec();
}
