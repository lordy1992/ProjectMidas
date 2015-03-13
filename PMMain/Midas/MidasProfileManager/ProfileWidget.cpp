#include "ProfileWidget.h"
#include <QLabel.h>
#include <QGroupBox.h>
#include <QListWidget.h>
#include <QPushButton.h>

ProfileWidget::ProfileWidget(QWidget *parent)
    : QWidget(parent)
{
    scrollArea = new QScrollArea();
    vlayout = new QVBoxLayout(scrollArea);
    vlayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(vlayout->widget());

    QLabel* profileLab = new QLabel("Profile: Default");
    vlayout->addWidget(profileLab);
    vlayout->setAlignment(profileLab, Qt::AlignTop);

    QGroupBox* grouper = new QGroupBox(tr("Sequence 1"));
    grouper->setMaximumSize(400, 310);

    QVBoxLayout* sequenceLayout = new QVBoxLayout();

    QLabel* sequenceTitle = new QLabel("Sequence Seq1 from LOCKED state");
    sequenceLayout->addWidget(sequenceTitle);

    QListWidget* sequences = new QListWidget();
    QListWidgetItem* item1 = new QListWidgetItem("tap ThumbToPinky");
    QListWidgetItem* item2 = new QListWidgetItem("tap WaveIn");
    QListWidgetItem* item3 = new QListWidgetItem("tap WaveOut");
    QListWidgetItem* item4 = new QListWidgetItem("tap Fist");

    sequences->setMaximumSize(256, 72);

    sequences->addItem(item1);
    sequences->addItem(item2);
    sequences->addItem(item3);
    sequences->addItem(item4);

    sequenceLayout->addWidget(sequences);

    QLabel* commandTitle = new QLabel("Command type STATE_CHANGE");
    sequenceLayout->addWidget(commandTitle);

    QListWidget* actions = new QListWidget();
    QListWidgetItem* action1 = new QListWidgetItem("mouseMode");

    actions->setMaximumSize(256, 72);

    actions->addItem(action1);
    
    sequenceLayout->addWidget(actions);

    QPushButton* editSequenceButton = new QPushButton();
    editSequenceButton->setMaximumSize(51, 23);
    editSequenceButton->setText("Edit");
    sequenceLayout->addWidget(editSequenceButton);

    grouper->setLayout(sequenceLayout);

    vlayout->addWidget(grouper);
    vlayout->setAlignment(grouper, Qt::AlignTop);

    QPushButton* addSequenceButton = new QPushButton();
    addSequenceButton->setMaximumSize(91, 23);

    addSequenceButton->setText("Add Sequence");

    vlayout->addWidget(addSequenceButton);
    vlayout->setAlignment(addSequenceButton, Qt::AlignTop);

    setLayout(vlayout);
}

ProfileWidget::~ProfileWidget()
{

}
