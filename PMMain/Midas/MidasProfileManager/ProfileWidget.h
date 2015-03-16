#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QScrollArea.h>
#include <QLayout.h>
#include "ProfileWriter.h"

class ProfileWidget : public QScrollArea
{
    Q_OBJECT

public:
    ProfileWidget(Profile profile, QWidget *parent = 0);
    ~ProfileWidget();

private:
    void drawProfile(Profile profile);
    void drawSequence(Sequence sequence);
    Profile prof;
    QVBoxLayout* vlayout;

};

#endif // PROFILEWIDGET_H
