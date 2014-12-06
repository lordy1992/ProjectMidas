#include "midasgui.h"
#include <string>

MidasGUI::MidasGUI(QThread *mainThread, QWidget *parent)
    : mainThread(mainThread), QDialog(parent)
{
    ui.setupUi(this);

    connect(mainThread, SIGNAL(outputCount(int)), this, SLOT(handleCount(int)));
}

MidasGUI::~MidasGUI()
{

}

void MidasGUI::handleCount(int count)
{
    ui.label->setText(QString::number(count));
}