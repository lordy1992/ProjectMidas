#include "ProfileCreatorGUI.h"
#include "ProfileWidget.h"

ProfileCreatorGUI::ProfileCreatorGUI()
{
    createMainGUI();
}


ProfileCreatorGUI::~ProfileCreatorGUI()
{
}

void ProfileCreatorGUI::newProfile()
{

}

void ProfileCreatorGUI::open()
{

}

void ProfileCreatorGUI::save()
{

}

void ProfileCreatorGUI::createMainGUI()
{
    tabWidget = new QTabWidget();
    addTabButton = new QToolButton(tabWidget);
    addTabButton->setCursor(Qt::ArrowCursor);
    addTabButton->setIcon(QIcon(":/MidasProfileManager/Resources/newTab.png"));
    addTabButton->setAutoRaise(true);

    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
    tabWidget->addTab(new ProfileWidget(), "Test");

    setCentralWidget(tabWidget);
    createActions();
    createMenu();
    //createToolBar();
}

void ProfileCreatorGUI::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new profile."));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newProfile()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing profile."));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the current profile."));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
}

void ProfileCreatorGUI::createMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
}

void ProfileCreatorGUI::createToolBar()
{
    toolBar = addToolBar(tr("File"));
    toolBar->addAction(newAction);
    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
}