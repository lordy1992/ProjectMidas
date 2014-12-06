#include "midasgui.h"
#include <string>

MidasGUI::MidasGUI(QThread *mainThread, QWidget *parent)
    : mainThread(mainThread), QDialog(parent)
{
    ui.setupUi(this);

    connect(mainThread, SIGNAL(outputCount(int)), this, SLOT(handleCount(int)));

    connect(mainThread, SIGNAL(emitString(std::str)), this, SLOT(displayMessage(std::str)));
}

MidasGUI::~MidasGUI()
{

}

void MidasGUI::handleCount(int count)
{
    ui.label->setText(QString::number(count));
}

void MidasGUI::displayMessage(std::string str)
{
    ui.label->setText(QString::fromStdString(str));
}