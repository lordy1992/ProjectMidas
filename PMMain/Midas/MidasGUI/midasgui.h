#ifndef MIDASGUI_H
#define MIDASGUI_H

#include <QtWidgets/QDialog>
#include <MidasThread.h>
#include "ui_midasgui.h"
#include <string>

Q_DECLARE_METATYPE(std::string)

class MidasGUI : public QDialog
{
    Q_OBJECT

public:
    MidasGUI(MidasThread *mainThread, QWidget *parent = 0);
    ~MidasGUI();

private:
    Ui::MidasGUIClass ui;
    MidasThread *mainThread;

public slots:
    void handleCount(int count);
    void displayMessage(std::string str);
    void handleXVeloc(int veloc);
    void handleYVeloc(int veloc);
};

#endif // MIDASGUI_H
