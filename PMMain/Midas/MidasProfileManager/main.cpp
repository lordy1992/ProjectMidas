#include <QtWidgets\QApplication.h>
#include "ProfileWriter.h"
#include "ProfileCreatorGUI.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Project Midas Profile Manager");
    ProfileCreatorGUI mainWindow;
    mainWindow.show();
    return a.exec();
}