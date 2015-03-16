#include "ProfileWidget.h"
#include <QLabel.h>
#include <QGroupBox.h>
#include <QListWidget.h>
#include <QPushButton.h>

ProfileWidget::ProfileWidget(Profile profile, QWidget *parent)
    : QScrollArea(parent)
{
    drawProfile(profile);
    prof = profile;
}

ProfileWidget::~ProfileWidget()
{

}

void ProfileWidget::drawProfile(Profile profile)
{
    this->setWidgetResizable(true);
    QWidget* holdingWidget = new QWidget(this);

    vlayout = new QVBoxLayout();
    vlayout->setAlignment(Qt::AlignTop);
    vlayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    std::string profileLabVal = "Profile: " + profile.profileName;
    QLabel* profileLab = new QLabel(QString(profileLabVal.c_str()));
    vlayout->addWidget(profileLab);
    vlayout->setAlignment(profileLab, Qt::AlignTop);

   // For each sequence.. drawSequence
    std::vector<Sequence> sequences = profile.sequences;
    for (int i = 0; i < sequences.size(); i++)
    {
        drawSequence(sequences[i]);
    }

    QPushButton* addSequenceButton = new QPushButton();
    addSequenceButton->setMaximumSize(91, 23);

    addSequenceButton->setText("Add Sequence");

    vlayout->addWidget(addSequenceButton);
    vlayout->setAlignment(addSequenceButton, Qt::AlignTop);
    vlayout->setSizeConstraint(QLayout::SetMinimumSize);

    holdingWidget->setLayout(vlayout);
    this->setWidget(holdingWidget);
}

void ProfileWidget::drawSequence(Sequence sequence)
{
    std::string title = "Sequence " + sequence.name;
    QGroupBox* grouper = new QGroupBox(tr(title.c_str()));
    grouper->setMaximumSize(400, 310);

    QVBoxLayout* sequenceLayout = new QVBoxLayout();

    std::string stateTitle = "Begins in state '" + sequence.state + "'";
    QLabel* sequenceTitle = new QLabel(QString(stateTitle.c_str()));
    sequenceLayout->addWidget(sequenceTitle);

    QListWidget* sequences = new QListWidget();

    std::vector<Gesture> gestures = sequence.gestures;
    std::vector<Gesture>::iterator it;

    sequences->setMaximumSize(256, 72);
    for (it = gestures.begin(); it != gestures.end(); it++)
    {
        std::string itemVal = it->type + " " + it->name;
        QListWidgetItem* item = new QListWidgetItem(QString(itemVal.c_str()));
        sequences->addItem(item);
    }

    sequenceLayout->addWidget(sequences);

    std::string cmdLabel = "Command type " + sequence.cmd.type;
    QLabel* commandTitle = new QLabel(QString(cmdLabel.c_str()));
    sequenceLayout->addWidget(commandTitle);

    QListWidget* actions = new QListWidget();
    actions->setMaximumSize(256, 72);

    std::vector<std::string> actionList = sequence.cmd.actions;
    std::vector<std::string>::iterator actionIt;

    for (actionIt = actionList.begin(); actionIt != actionList.end(); actionIt++)
    {
        QListWidgetItem* action = new QListWidgetItem(QString(actionIt->c_str()));
        actions->addItem(action);
    }

    sequenceLayout->addWidget(actions);

    QPushButton* editSequenceButton = new QPushButton();
    editSequenceButton->setMaximumSize(51, 23);
    editSequenceButton->setText("Edit");
    sequenceLayout->addWidget(editSequenceButton);

    grouper->setLayout(sequenceLayout);

    vlayout->addWidget(grouper);
    vlayout->setAlignment(grouper, Qt::AlignTop);
}