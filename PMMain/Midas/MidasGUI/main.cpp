#include "MidasThread.h"
#include "MouseCtrl.h"
#include "MainGUI.h"

#include <QtWidgets/QApplication>

#define TASK_BAR_ICON_PATH "Resources\\ProjectMidasLogo.ico"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MidasThread midasThread;

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");
    qRegisterMetaType<std::vector<sequenceProgressData> >("std::vector<sequenceProgressData>");

    MainGUI mainDisplay(&midasThread, MOVE_RATE_DEADZONE);

    midasThread.setMainGuiHandle(&mainDisplay);
    midasThread.start();

    QIcon *icon = new QIcon(TASK_BAR_ICON_PATH);
    mainDisplay.setWindowIcon(*icon);
    mainDisplay.show();
    return a.exec();
    system("PAUSE");
}
