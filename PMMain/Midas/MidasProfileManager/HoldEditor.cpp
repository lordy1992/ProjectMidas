#include "HoldEditor.h"

HoldEditor::HoldEditor(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.doneButton, SIGNAL(released()), this, SLOT(handleDone()));
}

HoldEditor::~HoldEditor()
{

}

Hold HoldEditor::getReturnHold()
{
    return returnHold;
}

bool HoldEditor::isAngleSet(QComboBox* positive, QComboBox* negative)
{
    return !(positive->currentText() == "none" && negative->currentText() == "none");
}

void HoldEditor::handleDone()
{
    returnHold.gesture = ui.gestureComboBox->currentText().toStdString();
    
    if (isAngleSet(ui.rollPositiveComboBox, ui.rollNegativeComboBox))
    {
        AngleAction roll;
        roll.type = "roll";
        roll.anglePositive = ui.rollPositiveComboBox->currentText().toStdString();
        roll.angleNegative = ui.rollNegativeComboBox->currentText().toStdString();

        returnHold.angles.push_back(roll);
    }

    if (isAngleSet(ui.pitchPositiveComboBox, ui.pitchNegativeComboBox))
    {
        AngleAction pitch;
        pitch.type = "pitch";
        pitch.anglePositive = ui.pitchPositiveComboBox->currentText().toStdString();
        pitch.angleNegative = ui.pitchNegativeComboBox->currentText().toStdString();

        returnHold.angles.push_back(pitch);
    }

    if (isAngleSet(ui.yawPositiveComboBox, ui.yawNegativeComboBox))
    {
        AngleAction yaw;
        yaw.type = "yaw";
        yaw.anglePositive = ui.yawPositiveComboBox->currentText().toStdString();
        yaw.angleNegative = ui.yawNegativeComboBox->currentText().toStdString();

        returnHold.angles.push_back(yaw);
    }

    this->accept();
}