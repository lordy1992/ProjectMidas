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
#include "ProfileWriter.h"
#include "ProfileWidget.h"

class ProfileCreatorGUI : public QMainWindow
{
    Q_OBJECT

public:
    ProfileCreatorGUI();
    ~ProfileCreatorGUI();

    QSize sizeHint() const;

private slots:
    void newFile();
    void open();
    void save();
    void newProfile();

private:
    void createMainGUI();
    void createActions();
    void createMenu();
    void createToolBar();
    void loadProfiles(std::string filename);

    QTabWidget* tabWidget;
    QToolBar* toolBar;
    QToolButton* addTabButton;
    QMenu* fileMenu;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    std::vector<ProfileWidget*> profileWidgets;
};

#endif /* PROFILE_CREATOR_GUI_H */