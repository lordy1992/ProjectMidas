#ifndef PROFILE_CREATOR_GUI_H
#define PROFILE_CREATOR_GUI_H

#include <QMainWindow.h>
#include <QMenuBar.h>
#include <QApplication.h>
#include <QToolButton.h>
#include <QTabWidget.h>
#include <QToolBar.h>
#include <QAction.h>
#include <QMenu.h>

class ProfileCreatorGUI : public QMainWindow
{
    Q_OBJECT

public:
    ProfileCreatorGUI();
    ~ProfileCreatorGUI();

    private slots:
    void newProfile();
    void open();
    void save();

private:
    void createMainGUI();
    void createActions();
    void createMenu();
    void createToolBar();

    QTabWidget* tabWidget;
    QToolBar* toolBar;
    QToolButton* addTabButton;
    QMenu* fileMenu;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;

};

#endif /* PROFILE_CREATOR_GUI_H */