#include "ProfileWidget.h"
#include "SequenceEditor.h"
#include "HoldEditor.h"

ProfileWidget::ProfileWidget(Profile profile, QWidget *parent)
    : QScrollArea(parent)
{
    prof = profile;
    drawProfile(profile);
}

ProfileWidget::~ProfileWidget()
{

}

Profile ProfileWidget::getProfile()
{
    return prof;
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

    holdMapper = new QSignalMapper(this);
    std::vector<Hold> holds = profile.holds;
    for (int i = 0; i < holds.size(); i++)
    {
        drawHold(holds[i], i);
    }
    connect(holdMapper, SIGNAL(mapped(int)), this, SLOT(holdEditButtonClicked(int)));

    QHBoxLayout* horLayout = new QHBoxLayout();
    horLayout->setAlignment(Qt::AlignLeft);
    QPushButton* addSequenceButton = new QPushButton();
    addSequenceButton->setMaximumSize(91, 23);
    addSequenceButton->setText("Add Sequence");

    QPushButton* addHoldButton = new QPushButton();
    addHoldButton->setMaximumSize(91, 23);
    addHoldButton->setText("Add Hold");
    horLayout->addWidget(addSequenceButton);
    horLayout->addWidget(addHoldButton);
    horLayout->setAlignment(addSequenceButton, Qt::AlignLeft);
    horLayout->setAlignment(addHoldButton, Qt::AlignLeft);

    vlayout->addLayout(horLayout);
    vlayout->setAlignment(addSequenceButton, Qt::AlignTop);
    vlayout->setSizeConstraint(QLayout::SetMinimumSize);

    connect(addSequenceButton, SIGNAL(released()), this, SLOT(addSequenceButtonClicked()));
    connect(addHoldButton, SIGNAL(released()), this, SLOT(addHoldButtonClicked()));

    holdingWidget->setLayout(vlayout);
    this->setWidget(holdingWidget);
}

void ProfileWidget::drawHold(Hold hold, int ind, bool insertBefore)
{
    std::string title = "Hold for gesture " + hold.gesture;
    QGroupBox* grouper = new QGroupBox(tr(title.c_str()));
    grouper->setMaximumSize(400, 310);

    QVBoxLayout* holdLayout = new QVBoxLayout();
    
    for (int i = 0; i < hold.angles.size(); i++)
    {
        AngleAction angle = hold.angles[i];
        std::string angleTitle = "Angle '" + angle.type + "':";
        QLabel* angleLabel = new QLabel(QString(angleTitle.c_str()));
        holdLayout->addWidget(angleLabel);

        std::string positiveText = "    On positive angle, do action '" + angle.anglePositive + "'";
        std::string negativeText = "    On negative angle, do action '" + angle.angleNegative + "'";
        QLabel* positiveLabel = new QLabel(QString(positiveText.c_str()));
        QLabel* negativeLabel = new QLabel(QString(negativeText.c_str()));

        holdLayout->addWidget(positiveLabel);
        holdLayout->addWidget(negativeLabel);
    }

    QPushButton* editHoldButton = new QPushButton();
    editHoldButton->setMaximumSize(51, 23);
    editHoldButton->setText("Edit");

    connect(editHoldButton, SIGNAL(released()), holdMapper, SLOT(map()));
    holdMapper->setMapping(editHoldButton, ind);
    holdLayout->addWidget(editHoldButton);

    grouper->setLayout(holdLayout);

    holdWidgets widgets;
    widgets.editHoldButton = editHoldButton;
    widgets.grouper = grouper;
    widgets.holdLayout = holdLayout;

    holdWidgetList.push_back(widgets);

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

void ProfileWidget::modifyHold(int ind, Hold hold)
{
    holdWidgets holdWidgets = holdWidgetList[ind];

    std::string title = "Hold for gesture " + hold.gesture;
    holdWidgets.grouper->setTitle(tr(title.c_str()));

    QLayoutItem *item;

    while ((item = holdWidgets.holdLayout->takeAt(0)) != 0)
    {
        holdWidgets.holdLayout->removeItem(item);

        if (item->widget() != 0)
        {
            item->widget()->hide();
            delete item->widget();
        }
        else
        {
            delete item;
        }
    }

    delete holdWidgets.holdLayout;

    QVBoxLayout* holdLayout = new QVBoxLayout();
    holdWidgetList[ind].holdLayout = holdLayout;

    for (int i = 0; i < hold.angles.size(); i++)
    {
        AngleAction angle = hold.angles[i];
        std::string angleTitle = "Angle '" + angle.type + "':";
        QLabel* angleLabel = new QLabel(QString(angleTitle.c_str()));
        holdLayout->addWidget(angleLabel);

        std::string positiveText = "    On positive angle, do action '" + angle.anglePositive + "'";
        std::string negativeText = "    On negative angle, do action '" + angle.angleNegative + "'";
        QLabel* positiveLabel = new QLabel(QString(positiveText.c_str()));
        QLabel* negativeLabel = new QLabel(QString(negativeText.c_str()));

        holdLayout->addWidget(positiveLabel);
        holdLayout->addWidget(negativeLabel);
    }

    QPushButton* editHoldButton = new QPushButton();
    editHoldButton->setMaximumSize(51, 23);
    editHoldButton->setText("Edit");
    holdWidgetList[ind].editHoldButton = editHoldButton;

    connect(editHoldButton, SIGNAL(released()), holdMapper, SLOT(map()));
    holdMapper->setMapping(editHoldButton, ind);
    holdLayout->addWidget(editHoldButton);

    holdWidgets.grouper->setLayout(holdLayout);
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

    std::string cmdLabel = "Command type " + sequence.cmds[0].type;
    QLabel* commandTitle = new QLabel(QString(cmdLabel.c_str()));
    sequenceLayout->addWidget(commandTitle);

    QListWidget* actions = new QListWidget();
    actions->setMaximumSize(256, 72);

	std::vector<std::string> actionList = sequence.cmds[0].actions;
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

	std::string cmdLabel = "Command type " + seq.cmds[0].type;
    seqWidgets.commandTitle->setText(QString(cmdLabel.c_str()));

	std::vector<std::string> actionList = seq.cmds[0].actions;
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
    std::vector<Sequence> otherSequences = prof.sequences;
    otherSequences.erase(otherSequences.begin() + id);
    editor.setOtherSequences(&otherSequences);
    if (editor.exec())
    {
        Sequence seq = editor.getSequence();
        prof.sequences[id] = seq;
        modifySequence(id, seq);
    }
}

void ProfileWidget::holdEditButtonClicked(int id)
{
    HoldEditor editor;
    if (editor.exec())
    {
        Hold hold = editor.getReturnHold();
        prof.holds[id] = hold;
        modifyHold(id, hold);
    }
}

void ProfileWidget::addSequenceButtonClicked()
{
    SequenceEditor editor;
    editor.setOtherSequences(&prof.sequences);
    if (editor.exec())
    {
        Sequence seq = editor.getSequence();
        prof.sequences.push_back(seq);
        drawSequence(seq, prof.sequences.size()-1, true);
    }
}

void ProfileWidget::addHoldButtonClicked()
{
    HoldEditor editor;
    if (editor.exec())
    {
        Hold hold = editor.getReturnHold();
        prof.holds.push_back(hold);
        drawHold(hold, prof.holds.size() - 1, true);
    }
}