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
    void formCommandComboBox();
    void formStateChangeActions();
    void formMouseActions();
    void formKybdActions();
    void formKybdGUIActions();

    Ui::SequenceEditor ui;
    Sequence returnSequence;

public slots:
    void handleAddGesture();
    void handleAddAction();
    void handleDone();
    void handleActivateCommandBox(const QString & text);
};

#endif // SEQUENCEEDITOR_H
