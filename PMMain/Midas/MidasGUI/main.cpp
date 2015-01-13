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

   // MidasGUI w(&midasThread);
   // MouseIndicator w;
    SequenceDisplayer w;

    w.show();
    return a.exec();
}
