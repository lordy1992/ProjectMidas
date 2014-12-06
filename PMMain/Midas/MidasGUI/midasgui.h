#ifndef MIDASGUI_H
#define MIDASGUI_H

#include <QtWidgets/QDialog>
#include "ui_midasgui.h"

class MidasGUI : public QDialog
{
    Q_OBJECT

public:
    MidasGUI(QWidget *parent = 0);
    ~MidasGUI();

private:
    Ui::MidasGUIClass ui;
};

#endif // MIDASGUI_H
