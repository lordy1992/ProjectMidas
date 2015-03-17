#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QScrollArea.h>
#include <QLayout.h>
#include "ProfileWriter.h"
#include <QSignalMapper>

class ProfileWidget : public QScrollArea
{
    Q_OBJECT

public:
    ProfileWidget(Profile profile, QWidget *parent = 0);
    ~ProfileWidget();

private:
    void drawProfile(Profile profile);
    void drawSequence(Sequence sequence, int ind);
    Profile prof;
    QVBoxLayout* vlayout;
    QSignalMapper* mapper;

public slots:
    void editButtonClicked(int id);

};

#endif // PROFILEWIDGET_H
