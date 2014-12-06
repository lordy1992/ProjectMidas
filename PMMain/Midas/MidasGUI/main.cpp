#include "midasgui.h"
#include "GuiThread.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MidasGUI w;

    GUIThread gui(&a, &w);
    gui.start();

    //w.show();
    //return a.exec();

    for (;;) {}

    return 0;
}
