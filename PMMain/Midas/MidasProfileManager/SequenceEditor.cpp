#include "SequenceEditor.h"
#include <QListWidgetItem>

SequenceEditor::SequenceEditor(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.addGestureButton, SIGNAL(released()), this, SLOT(handleAddGesture()));
    connect(ui.addActionButton, SIGNAL(released()), this, SLOT(handleAddAction()));
    connect(ui.doneButton, SIGNAL(released()), this, SLOT(handleDone()));
}

SequenceEditor::~SequenceEditor()
{

}

Sequence SequenceEditor::getSequence()
{
    return returnSequence;
}

void SequenceEditor::handleAddGesture()
{
    QString gestureType = ui.gestureTypeComboBox->currentText();
    QString gesture = ui.gestureComboBox->currentText();
    QString row = gestureType + " " + gesture;
    ui.gestureList->addItem(row);
}

void SequenceEditor::handleAddAction()
{
    QString action = ui.actionComboBox->currentText();
    ui.actionList->addItem(action);
}

void SequenceEditor::handleDone()
{
    returnSequence.name = ui.lineEdit->text().toStdString();
    returnSequence.state = ui.stateComboBox->currentText().toStdString();

    std::vector<Gesture> gestures;
    for (int row = 0; row < ui.gestureList->count(); row++)
    {
        QListWidgetItem* item = ui.gestureList->item(row);
        Gesture gest;
        QStringList gestStr = item->text().split(" ");
        gest.name = gestStr[1].toStdString();
        gest.type = gestStr[0].toStdString();
        gestures.push_back(gest);
    }

    returnSequence.gestures = gestures;

    std::vector<std::string> actions;
    for (int row = 0; row < ui.actionList->count(); row++)
    {
        QListWidgetItem* item = ui.actionList->item(row);
        std::string action;
        action = item->text().toStdString();
        actions.push_back(action);
    }

    returnSequence.cmd.type = ui.commandComboBox->currentText().toStdString();
    returnSequence.cmd.actions = actions;

    this->accept();
}