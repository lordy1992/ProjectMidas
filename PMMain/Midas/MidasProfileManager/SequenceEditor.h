#ifndef SEQUENCEEDITOR_H
#define SEQUENCEEDITOR_H

#include <QDialog>
#include "ui_SequenceEditor.h"

class SequenceEditor : public QDialog
{
    Q_OBJECT

public:
    SequenceEditor(QWidget *parent = 0);
    ~SequenceEditor();

private:
    Ui::SequenceEditor ui;
};

#endif // SEQUENCEEDITOR_H
