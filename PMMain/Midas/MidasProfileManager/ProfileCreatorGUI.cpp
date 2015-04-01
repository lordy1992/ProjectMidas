#include "ProfileCreatorGUI.h"
#include <QInputDialog.h>
#include <QFileDialog.h>

#define DEFAULT_WIDTH 400
#define DEFAULT_HEIGHT 480

ProfileCreatorGUI::ProfileCreatorGUI()
{
    createMainGUI();
}


ProfileCreatorGUI::~ProfileCreatorGUI()
{
}

void ProfileCreatorGUI::newFile()
{
    profileWidgets.clear();
    tabWidget->clear();

    Profile newProfile;
    newProfile.profileName = "default";
    ProfileWidget* profileWidget = new ProfileWidget(newProfile);
    profileWidgets.push_back(profileWidget);
    tabWidget->addTab(profileWidget, "default");
}

void ProfileCreatorGUI::newProfile()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New Profile"),
        tr("Profile name:"), QLineEdit::Normal, QString(), &ok);

    if (ok && !text.isEmpty())
    {
        Profile profile;
        profile.profileName = text.toStdString();
        ProfileWidget* profileWidget = new ProfileWidget(profile);
        profileWidgets.push_back(profileWidget);
        tabWidget->addTab(profileWidget, QString(profile.profileName.c_str()));  
    }
}

void ProfileCreatorGUI::open()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("XML files(*.xml)"));
    if (!filename.isNull())
    {
        ProfileWriter writer;
        loadProfiles(filename.toStdString());
    }
}

void ProfileCreatorGUI::save()
{
    std::vector<Profile> profiles;
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        profiles.push_back(profileWidgets[i]->getProfile());
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), tr("XML files(*.xml)"));

    if (!filename.isNull())
    {
        std::string title = filename.toStdString() + " - Project Midas Profile Manager";
        setWindowTitle(QString(title.c_str()));

        ProfileWriter writer;
        writer.writeProfiles(filename.toStdString(), profiles);
    }
}

void ProfileCreatorGUI::loadProfiles(std::string filename)
{
    ProfileWriter writer;
    std::vector<Profile> profiles = writer.loadProfilesFromFile(filename);

    std::string title = filename + " - Project Midas Profile Manager";
    setWindowTitle(QString(title.c_str()));

    tabWidget->clear();
    std::vector<Profile>::iterator it;
    for (it = profiles.begin(); it != profiles.end(); it++)
    {
        ProfileWidget* profileWidget = new ProfileWidget(*it);
        profileWidgets.push_back(profileWidget);
        tabWidget->addTab(profileWidget, QString(it->profileName.c_str()));
    }
}

void ProfileCreatorGUI::createMainGUI()
{
    tabWidget = new QTabWidget();
    addTabButton = new QToolButton(tabWidget);
    addTabButton->setCursor(Qt::ArrowCursor);
    addTabButton->setIcon(QIcon(":/MidasProfileManager/Resources/newTab.png"));
    addTabButton->setAutoRaise(true);

    connect(addTabButton, SIGNAL(clicked()), this, SLOT(newProfile()));

    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);

    setWindowTitle("Project Midas Profile Manager");

    ProfileWidget* profileWidget = new ProfileWidget(Profile());
    profileWidgets.push_back(profileWidget);
    tabWidget->addTab(profileWidget, "default");

    setCentralWidget(tabWidget);
    createActions();
    createMenu();
}

QSize ProfileCreatorGUI::sizeHint() const
{
    return QSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

void ProfileCreatorGUI::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new profile."));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

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