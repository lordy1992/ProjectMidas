#include "SequenceEditor.h"
#include <QListWidgetItem>

SequenceEditor::SequenceEditor(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    formCommandComboBox();

    connect(ui.addGestureButton, SIGNAL(released()), this, SLOT(handleAddGesture()));
    connect(ui.addActionButton, SIGNAL(released()), this, SLOT(handleAddAction()));
    connect(ui.doneButton, SIGNAL(released()), this, SLOT(handleDone()));
    connect(ui.commandComboBox, SIGNAL(activated(const QString &)), this, SLOT(handleActivateCommandBox(const QString &)));
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

void SequenceEditor::handleActivateCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions();
    }
    else if (text == "keyboardGui")
    {
        formKybdGUIActions();
    }
    else if (text == "mouse")
    {
        formMouseActions();
    }
    else if (text == "stateChange")
    {
        formStateChangeActions();
    }
}

void SequenceEditor::formCommandComboBox()
{
    ui.commandComboBox->clear();

    ui.commandComboBox->addItem(QString("keyboard"));
    ui.commandComboBox->addItem(QString("keyboardGui"));
    ui.commandComboBox->addItem(QString("mouse"));
    ui.commandComboBox->addItem(QString("stateChange"));
}

void SequenceEditor::formStateChangeActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("lockMode"));
    ui.actionComboBox->addItem(QString("mouseMode"));
    ui.actionComboBox->addItem(QString("keyboardMode"));
    ui.actionComboBox->addItem(QString("gestureMode"));
    ui.actionComboBox->addItem(QString("gestureHoldOne"));
    ui.actionComboBox->addItem(QString("gestureHoldTwo"));
    ui.actionComboBox->addItem(QString("gestureHoldThree"));
    ui.actionComboBox->addItem(QString("gestureHoldFour"));
    ui.actionComboBox->addItem(QString("gestureHoldFive"));
}

void SequenceEditor::formMouseActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("leftClick"));
    ui.actionComboBox->addItem(QString("rightClick"));
    ui.actionComboBox->addItem(QString("middleClick"));
    ui.actionComboBox->addItem(QString("leftHold"));
    ui.actionComboBox->addItem(QString("rightHold"));
    ui.actionComboBox->addItem(QString("middleHold"));
    ui.actionComboBox->addItem(QString("moveLeft"));
    ui.actionComboBox->addItem(QString("moveRight"));
    ui.actionComboBox->addItem(QString("moveUp"));
    ui.actionComboBox->addItem(QString("moveDown"));
    ui.actionComboBox->addItem(QString("moveHor"));
    ui.actionComboBox->addItem(QString("moveVert"));
    ui.actionComboBox->addItem(QString("scrollLeft"));
    ui.actionComboBox->addItem(QString("scrollRight"));
    ui.actionComboBox->addItem(QString("scrollUp"));
    ui.actionComboBox->addItem(QString("scrollDown"));
    ui.actionComboBox->addItem(QString("leftRelease"));
    ui.actionComboBox->addItem(QString("rightRelease"));
    ui.actionComboBox->addItem(QString("middleRelease"));
    ui.actionComboBox->addItem(QString("releaseLrmButs"));
}

void SequenceEditor::formKybdActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("undo"));
    ui.actionComboBox->addItem(QString("redo"));
    ui.actionComboBox->addItem(QString("zoomIn"));
    ui.actionComboBox->addItem(QString("zoomOut"));
    ui.actionComboBox->addItem(QString("zoom100"));
    ui.actionComboBox->addItem(QString("escape"));
    ui.actionComboBox->addItem(QString("enter"));
    ui.actionComboBox->addItem(QString("tab"));
    ui.actionComboBox->addItem(QString("switchWinForward"));
    ui.actionComboBox->addItem(QString("switchWinReverse"));
    ui.actionComboBox->addItem(QString("copy"));
    ui.actionComboBox->addItem(QString("paste"));
    ui.actionComboBox->addItem(QString("cut"));
    ui.actionComboBox->addItem(QString("fileMenu"));
    ui.actionComboBox->addItem(QString("newBrowser"));
    ui.actionComboBox->addItem(QString("gotoAddrBar"));
    ui.actionComboBox->addItem(QString("lockDesktop"));
    ui.actionComboBox->addItem(QString("editMenu"));
    ui.actionComboBox->addItem(QString("viewMenu"));
    ui.actionComboBox->addItem(QString("winHome"));
    ui.actionComboBox->addItem(QString("hideApps"));
    ui.actionComboBox->addItem(QString("control"));
    ui.actionComboBox->addItem(QString("volumeUp"));
    ui.actionComboBox->addItem(QString("volumeDown"));
    ui.actionComboBox->addItem(QString("backspace"));
    ui.actionComboBox->addItem(QString("upArrow"));
    ui.actionComboBox->addItem(QString("downArrow"));
    ui.actionComboBox->addItem(QString("rightArrow"));
    ui.actionComboBox->addItem(QString("leftArrow"));
    ui.actionComboBox->addItem(QString("none"));
}

void SequenceEditor::formKybdGUIActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("swapRingFocus"));
    ui.actionComboBox->addItem(QString("changeWheels"));
    ui.actionComboBox->addItem(QString("select"));
    ui.actionComboBox->addItem(QString("holdSelect"));
}