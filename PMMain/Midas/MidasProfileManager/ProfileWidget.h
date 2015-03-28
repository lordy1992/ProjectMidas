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
#include <QPushButton.h>

struct sequenceWidgets {
    QGroupBox* grouper;
    QLabel* stateTitle;
    QListWidget* sequences;
    QLabel* commandTitle;
    QListWidget* actions;
};

struct holdWidgets {
    QGroupBox* grouper;
    QVBoxLayout* holdLayout;
    QPushButton* editHoldButton;
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
    void drawHold(Hold hold, int ind, bool insertBefore = false);
    void drawSequence(Sequence sequence, int ind, bool insertBefore = false);
    void modifySequence(int ind, Sequence seq);
    void modifyHold(int ind, Hold hold);
    Profile prof;
    QVBoxLayout* vlayout;
    QSignalMapper* mapper;
    QSignalMapper* holdMapper;
    std::vector<sequenceWidgets> seqWidgetList;
    std::vector<holdWidgets> holdWidgetList;

public slots:
    void editButtonClicked(int id);
    void holdEditButtonClicked(int id);
    void addSequenceButtonClicked();
    void addHoldButtonClicked();

};

#endif // PROFILEWIDGET_H
