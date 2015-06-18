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
    //mouseIndicator = new MouseIndicator(mainThread, deadZoneRad, MOUSE_INDICATOR_SIZE,
    //    MOUSE_INDICATOR_SIZE, this);
	infoIndicator = new InfoIndicator(INFO_INDICATOR_WIDTH, INFO_INDICATOR_HEIGHT, this);
    sequenceDisplayer = new SequenceDisplayer(this);
	poseDisplayer = new PoseDisplayer(MOUSE_INDICATOR_SIZE + 25, MOUSE_INDICATOR_SIZE + 25, this);
	poseDisplayer2 = new PoseDisplayer(SPECIFIC_PROFILE_ICON_SIZE, SPECIFIC_PROFILE_ICON_SIZE, this);
	poseDisplayer3 = new PoseDisplayer(SPECIFIC_PROFILE_ICON_SIZE, SPECIFIC_PROFILE_ICON_SIZE, this);

    //setWindowFlags(windowFlags() | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

	// create main layout and add sequences (they are at the top and constantly go in/out of view)
    layout = new QVBoxLayout;
	layout->addWidget(sequenceDisplayer);

	// create HBox for specific profile icons: Change this icon to be specific to your app
	QHBoxLayout *profileIconLayout = new QHBoxLayout;
	profileIconLayout->addWidget(poseDisplayer2);
	profileIconLayout->addWidget(poseDisplayer3);

	QVBoxLayout *leftBoxLayout = new QVBoxLayout;
	leftBoxLayout->addWidget(infoIndicator);
	leftBoxLayout->addItem(profileIconLayout);

	QHBoxLayout *mainBoxLayout = new QHBoxLayout;
	mainBoxLayout->addItem(leftBoxLayout);
	mainBoxLayout->addWidget(poseDisplayer);
	mainBoxLayout->setAlignment(Qt::AlignRight);

    //QHBoxLayout *boxLayout = new QHBoxLayout;
    //boxLayout->setSpacing(INFO_INDICATOR_WIDTH - MOUSE_INDICATOR_SIZE*2);
	//
    //layout->addWidget(infoIndicator);
    //
    //std::vector<profile>* profiles = pm->getProfiles();
    //std::vector<profile>::iterator it;
    //int profileHeights = 0;
    //for (it = profiles->begin(); it != profiles->end(); it++)
    //{
    //    ProfileDisplayer* displayer = new ProfileDisplayer(it->profileName, PROF_INDICATOR_WIDTH, PROF_INDICATOR_HEIGHT, this);
    //    profileHeights += displayer->height();
    //    profileWidgets.push_back(displayer);
    //    layout->addWidget(displayer, 0, Qt::AlignRight);
    //}
	//
    //boxLayout->addWidget(poseDisplayer, 1, Qt::AlignRight);    
    //boxLayout->addWidget(mouseIndicator, 0, Qt::AlignRight);
    //layout->addLayout(boxLayout);
	//
    //layout->setAlignment(infoIndicator, Qt::AlignRight);
    //layout->setStretchFactor(infoIndicator, 0);

	layout->addItem(mainBoxLayout);
     
    setLayout(layout);

    totalWidth = std::max(sequenceDisplayer->width(), 
                        infoIndicator->width()); // this is wrong currently
    totalHeight = sequenceDisplayer->height() + poseDisplayer->height();

    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - totalWidth - SCREEN_RIGHT_BUFFER,
        screen.bottom() - totalHeight - SCREEN_BOTTOM_BUFFER,
        totalWidth, totalHeight);
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
    delete layout;
    layout = NULL;
}

void MainGUI::connectSignallerToProfileWidgets(ProfileSignaller* signaller)
{
	QMetaObject::Connection conn;
    for (int i = 0; i < profileWidgets.size(); i++)
    {
		conn = QObject::connect(profileWidgets[i], SIGNAL(emitChangeProfile(QString)), signaller, SLOT(handleProfilePress(QString)));
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
	QObject::connect(signaller, SIGNAL(emitPoseImages(std::vector<sequenceImageSet>)),
		poseDisplayer2, SLOT(handlePoseImages(std::vector<sequenceImageSet>)));
	QObject::connect(signaller, SIGNAL(emitPoseImages(std::vector<sequenceImageSet>)),
		poseDisplayer3, SLOT(handlePoseImages(std::vector<sequenceImageSet>)));
}

