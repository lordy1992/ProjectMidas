#ifndef SEQUENCEEDITOR_H
#define SEQUENCEEDITOR_H

#include <QDialog>
#include "ui_SequenceEditor.h"
#include "ProfileWriter.h"

class SequenceEditor : public QDialog
{
    Q_OBJECT

public:
    SequenceEditor(QWidget *parent = 0);
    ~SequenceEditor();

    Sequence getSequence();

private:

    Ui::SequenceEditor ui;
    Sequence returnSequence;

public slots:
    void handleAddGesture();
    void handleAddAction();
    void handleDone();
};

#endif // SEQUENCEEDITOR_H
