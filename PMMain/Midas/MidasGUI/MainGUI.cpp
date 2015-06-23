#define NOMINMAX

#include "MainGUI.h"
#include "ProfileDisplayer.h"
#include <QApplication.h>
#include <QDesktopWidget.h>
#include <algorithm>
#include <QEvent.h>

#define SCREEN_RIGHT_BUFFER    20 
#define SCREEN_BOTTOM_BUFFER   30

MainGUI::MainGUI(MidasThread *mainThread, ProfileManager *pm, int deadZoneRad)
    : DraggableWidget(NULL, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
    mouseIndicator = new MouseIndicator(mainThread, deadZoneRad, MOUSE_INDICATOR_SIZE,
        MOUSE_INDICATOR_SIZE, this);
    infoIndicator = new InfoIndicator(INFO_INDICATOR_WIDTH, 
        INFO_INDICATOR_HEIGHT, this);
    sequenceDisplayer = new SequenceDisplayer(this);
    poseDisplayer = new PoseDisplayer(MOUSE_INDICATOR_SIZE, MOUSE_INDICATOR_SIZE, this);
    distanceDisplayer = new DistanceWidget(mainThread, INFO_INDICATOR_WIDTH,
        DISTANCE_DISPLAY_HEIGHT, this);

    //setWindowFlags(windowFlags() | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

    layout = new QVBoxLayout;
    QHBoxLayout *boxLayout = new QHBoxLayout;
    boxLayout->setSpacing(INFO_INDICATOR_WIDTH - MOUSE_INDICATOR_SIZE*2);

    layout->addWidget(sequenceDisplayer);
    layout->addWidget(infoIndicator);
    
    std::vector<profile>* profiles = pm->getProfiles();
    std::vector<profile>::iterator it;
    int profileHeights = 0;
    for (it = profiles->begin(); it != profiles->end(); it++)
    {
        ProfileDisplayer* displayer = new ProfileDisplayer(it->profileName, PROF_INDICATOR_WIDTH, PROF_INDICATOR_HEIGHT, this);
        profileHeights += displayer->height();
        profileWidgets.push_back(displayer);
        layout->addWidget(displayer, 0, Qt::AlignRight);
    }
	
    layout->addWidget(distanceDisplayer);

    boxLayout->addWidget(poseDisplayer, 1, Qt::AlignRight);    
    boxLayout->addWidget(mouseIndicator, 0, Qt::AlignRight);
    layout->addLayout(boxLayout);

    layout->setAlignment(infoIndicator, Qt::AlignRight);
    layout->setStretchFactor(infoIndicator, 0);
    layout->setAlignment(distanceDisplayer, Qt::AlignRight);
    layout->setStretchFactor(distanceDisplayer, 0);
     
    setLayout(layout);

    keyboard = new KeyboardWidget(mainThread);
    keyboard->addWheels(mainThread->getKybrdRingData());

    totalWidth = std::max(sequenceDisplayer->width(), 
                        std::max(infoIndicator->width(),
                        std::max(mouseIndicator->width(), distanceDisplayer->width())));
    totalHeight = sequenceDisplayer->height() + infoIndicator->height() + 
        mouseIndicator->height() + profileHeights + distanceDisplayer->height();

    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - totalWidth - SCREEN_RIGHT_BUFFER,
        screen.bottom() - totalHeight - SCREEN_BOTTOM_BUFFER,
        totalWidth, totalHeight);
}

void MainGUI::toggleKeyboard()
{
    if (keyboard->isVisible())
    {
        keyboard->setVisible(false);
    }
    else
    {
        keyboard->setVisible(true);
    }
}

MainGUI::~MainGUI()
{
    delete mouseIndicator;
    mouseIndicator = NULL;
    delete infoIndicator;
    infoIndicator = NULL;
    delete sequenceDisplayer;
    sequenceDisplayer = NULL;
    delete poseDisplayer;
    poseDisplayer = NULL;
    delete distanceDisplayer;
    distanceDisplayer = NULL;
    delete layout;
    layout = NULL;
}

void MainGUI::connectSignallerToProfileWidgets(ProfileSignaller* signaller)
{
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        QObject::connect(profileWidgets[i], SIGNAL(emitChangeProfile(QString)), signaller, SLOT(handleProfilePress(QString)));
    }
}

void MainGUI::connectSignallerToInfoIndicator(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitStateString(QString)),
        infoIndicator, SLOT(handleUpdateState(QString)));
    QObject::connect(infoIndicator, SIGNAL(emitShowAllToggle(bool)),
        signaller, SLOT(handleShowAllToggle(bool)));
}

void MainGUI::connectSignallerToSequenceDisplayer(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitRegisterSequence(int, QString, std::vector<sequenceImageSet>)),
        sequenceDisplayer, SLOT(registerSequenceImages(int, QString, std::vector<sequenceImageSet>)));

    QObject::connect(signaller, SIGNAL(emitShowSequences(std::vector<sequenceProgressData>)),
        sequenceDisplayer, SLOT(showSequences(std::vector<sequenceProgressData>)));
}

void MainGUI::connectSignallerToPoseDisplayer(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitPoseImages(std::vector<sequenceImageSet>)),
        poseDisplayer, SLOT(handlePoseImages(std::vector<sequenceImageSet>)));
}

void MainGUI::connectSignallerToKeyboardToggle(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitToggleKeyboard()),
        this, SLOT(toggleKeyboard()));
}
