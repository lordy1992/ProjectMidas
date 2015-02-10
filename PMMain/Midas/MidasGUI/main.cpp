#include "MidasThread.h"
#include "MouseCtrl.h"
#include "MainGUI.h"
#include "KeyboardWidget.h"
#include "KeyboardSettingsReader.h"

#include <QtWidgets/QApplication>

#define TASK_BAR_ICON_PATH "Resources\\ProjectMidasLogo.ico"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::vector<ringData> kybrdRingData;
    KeyboardSettingsReader readFile;
    readFile.readKeyboardSetupFile(kybrdRingData);

    MidasThread midasThread(&kybrdRingData);

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");
    qRegisterMetaType<std::vector<sequenceProgressData> >("std::vector<sequenceProgressData>");

    MainGUI mainDisplay(&midasThread, MOVE_RATE_DEADZONE);
    midasThread.setMainGuiHandle(&mainDisplay);
    midasThread.start();

    // TODO: This is a hack. It fixes the issue with the widgets not staying on top, but
    // we still have to investigate why this works. Could be a bug with how we are creating
    // our widgets, or it could be an issue in Qt (or we do not fully understand how Qt handles 
    // showing widgets and windows that are always on top).
    QLabel dummyLabel;

    dummyLabel.show();
    dummyLabel.setVisible(false);
    // End hack

    mainDisplay.show();
    return a.exec();
    system("PAUSE");
}
