#include "midasgui.h"
#include <string>

MidasGUI::MidasGUI(MidasThread *mainThread, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    this->mainThread = mainThread;

    connect(mainThread, SIGNAL(outputCount(int)), this, SLOT(handleCount(int)));

    qRegisterMetaType<std::string>("std::String");
    connect(mainThread, SIGNAL(emitString(std::string)), this, SLOT(displayMessage(std::string)));
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