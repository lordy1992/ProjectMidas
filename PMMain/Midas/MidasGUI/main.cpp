#include "midasgui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MidasGUI w;
    w.show();
    return a.exec();
}
