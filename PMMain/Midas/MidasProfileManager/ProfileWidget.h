#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QScrollArea.h>
#include <QLayout.h>

class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    ProfileWidget(QWidget *parent = 0);
    ~ProfileWidget();

private:
    QScrollArea* scrollArea;
    QVBoxLayout* vlayout;

};

#endif // PROFILEWIDGET_H
