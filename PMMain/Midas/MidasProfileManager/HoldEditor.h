#ifndef HOLDEDITOR_H
#define HOLDEDITOR_H

#include <QDialog>
#include "ProfileWriter.h"
#include "ui_HoldEditor.h"

class HoldEditor : public QDialog
{
    Q_OBJECT

public:
    HoldEditor(QWidget *parent = 0);
    ~HoldEditor();

    Hold getReturnHold();

private:
    Ui::HoldEditor ui;

    Hold returnHold;
    bool isAngleSet(QComboBox* positive, QComboBox* negative);

public slots:
    void handleDone();
};

#endif // HOLDEDITOR_H
