#ifndef MIDASGUI_H
#define MIDASGUI_H

#include <QtWidgets/QDialog>
#include <qthread.h>
#include "ui_midasgui.h"
#include <string>

class MidasGUI : public QDialog
{
    Q_OBJECT

public:
    MidasGUI(QThread *mainThread, QWidget *parent = 0);
    ~MidasGUI();

private:
    Ui::MidasGUIClass ui;
    QThread mainThread;

private slots:
    void handleCount(int count);
    void displayMessage(std::string str);
};

#endif // MIDASGUI_H
