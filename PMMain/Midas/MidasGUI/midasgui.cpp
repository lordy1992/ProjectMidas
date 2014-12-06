#include "midasgui.h"

MidasGUI::MidasGUI(QThread *mainThread, QWidget *parent)
    : mainThread(mainThread), QDialog(parent)
{
    ui.setupUi(this);
}

MidasGUI::~MidasGUI()
{

}
