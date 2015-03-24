#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QScrollArea.h>
#include <QLayout.h>
#include "ProfileWriter.h"
#include <QSignalMapper>
#include <QGroupBox.h>
#include <QLabel.h>
#include <QListWidget.h>

struct sequenceWidgets {
    QGroupBox* grouper;
    QLabel* stateTitle;
    QListWidget* sequences;
    QLabel* commandTitle;
    QListWidget* actions;
};

class ProfileWidget : public QScrollArea
{
    Q_OBJECT

public:
    ProfileWidget(Profile profile, QWidget *parent = 0);
    ~ProfileWidget();

    Profile getProfile();

private:
    void drawProfile(Profile profile);
    void drawSequence(Sequence sequence, int ind, bool insertBefore = false);
    void modifySequence(int ind, Sequence seq);
    Profile prof;
    QVBoxLayout* vlayout;
    QSignalMapper* mapper;
    std::vector<sequenceWidgets> seqWidgetList;

public slots:
    void editButtonClicked(int id);
    void addSequenceButtonClicked();

};

#endif // PROFILEWIDGET_H
