#ifndef MIDASGUI_H
#define MIDASGUI_H

#include <QtWidgets/QDialog>
#include <qthread.h>
#include "ui_midasgui.h"

class MidasGUI : public QDialog
{
    Q_OBJECT

public:
    MidasGUI(QThread *mainThread, QWidget *parent = 0);
    ~MidasGUI();

private:
    Ui::MidasGUIClass ui;
    QThread mainThread;
};

#endif // MIDASGUI_H
