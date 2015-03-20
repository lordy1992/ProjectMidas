#include "ProfileWidget.h"
#include <QPushButton.h>
#include "SequenceEditor.h"

ProfileWidget::ProfileWidget(Profile profile, QWidget *parent)
    : QScrollArea(parent)
{
    prof = profile;
    drawProfile(profile);
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
    mapper = new QSignalMapper(this);
    std::vector<Sequence> sequences = profile.sequences;
    for (int i = 0; i < sequences.size(); i++)
    {
        drawSequence(sequences[i], i);
    }
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(editButtonClicked(int)));

    QPushButton* addSequenceButton = new QPushButton();
    addSequenceButton->setMaximumSize(91, 23);

    addSequenceButton->setText("Add Sequence");

    vlayout->addWidget(addSequenceButton);
    vlayout->setAlignment(addSequenceButton, Qt::AlignTop);
    vlayout->setSizeConstraint(QLayout::SetMinimumSize);

    connect(addSequenceButton, SIGNAL(released()), this, SLOT(addSequenceButtonClicked()));

    holdingWidget->setLayout(vlayout);
    this->setWidget(holdingWidget);
}

void ProfileWidget::drawSequence(Sequence sequence, int ind, bool insertBefore)
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

    sequenceWidgets seqWidgets;
    seqWidgets.actions = actions; 
    seqWidgets.commandTitle = commandTitle;
    seqWidgets.sequences = sequences;
    seqWidgets.stateTitle = sequenceTitle;
    seqWidgets.grouper = grouper;

    seqWidgetList.push_back(seqWidgets);

    QPushButton* editSequenceButton = new QPushButton();
    editSequenceButton->setMaximumSize(51, 23);
    editSequenceButton->setText("Edit");

    connect(editSequenceButton, SIGNAL(released()), mapper, SLOT(map()));
    mapper->setMapping(editSequenceButton, ind);
    sequenceLayout->addWidget(editSequenceButton);

    grouper->setLayout(sequenceLayout);

    if (insertBefore)
    {
        vlayout->insertWidget(vlayout->count() - 1, grouper);
    }
    else
    {
        vlayout->addWidget(grouper);
    }

    vlayout->setAlignment(grouper, Qt::AlignTop);
}

void ProfileWidget::modifySequence(int ind, Sequence seq)
{
    sequenceWidgets seqWidgets = seqWidgetList[ind];

    std::string title = "Sequence " + seq.name;
    seqWidgets.grouper->setTitle(tr(title.c_str()));

    std::string stateTitle = "Begins in state '" + seq.state + "'";
    seqWidgets.stateTitle->setText(QString(stateTitle.c_str()));

    std::vector<Gesture> gestures = seq.gestures;
    std::vector<Gesture>::iterator it;

    seqWidgets.sequences->clear();
    for (it = gestures.begin(); it != gestures.end(); it++)
    {
        std::string itemVal = it->type + " " + it->name;
        QListWidgetItem* item = new QListWidgetItem(QString(itemVal.c_str()));
        seqWidgets.sequences->addItem(item);
    }

    std::string cmdLabel = "Command type " + seq.cmd.type;
    seqWidgets.commandTitle->setText(QString(cmdLabel.c_str()));

    std::vector<std::string> actionList = seq.cmd.actions;
    std::vector<std::string>::iterator actionIt;

    seqWidgets.actions->clear();
    for (actionIt = actionList.begin(); actionIt != actionList.end(); actionIt++)
    {
        QListWidgetItem* action = new QListWidgetItem(QString(actionIt->c_str()));
        seqWidgets.actions->addItem(action);
    }
}

void ProfileWidget::editButtonClicked(int id)
{
    SequenceEditor editor;
    if (editor.exec())
    {
        Sequence seq = editor.getSequence();
        prof.sequences[id] = seq;
        modifySequence(id, seq);
    }
}

void ProfileWidget::addSequenceButtonClicked()
{
    SequenceEditor editor;
    if (editor.exec())
    {
        Sequence seq = editor.getSequence();
        prof.sequences.push_back(seq);
        drawSequence(seq, prof.sequences.size()-1, true);
    }
}