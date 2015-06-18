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
	infoIndicator = new InfoIndicator(INFO_INDICATOR_WIDTH, INFO_INDICATOR_HEIGHT, this);
    sequenceDisplayer = new SequenceDisplayer(this);
	poseDisplayer = new PoseDisplayer(MOUSE_INDICATOR_SIZE + 30, MOUSE_INDICATOR_SIZE + 30, this);

	setupProfileIcons();

    //setWindowFlags(windowFlags() | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

	// create main layout and add sequences (they are at the top and constantly go in/out of view)
    layout = new QVBoxLayout;
	layout->addWidget(sequenceDisplayer);

	// create HBox for specific profile icons: Change this icon to be specific to your app
	QHBoxLayout *profileIconLayout = new QHBoxLayout;
	profileIconLayout->addWidget(icon0);
	profileIconLayout->addWidget(icon1);

	QVBoxLayout *leftBoxLayout = new QVBoxLayout;
	leftBoxLayout->addWidget(infoIndicator);
	leftBoxLayout->addItem(profileIconLayout);

	QHBoxLayout *mainBoxLayout = new QHBoxLayout;
	mainBoxLayout->addItem(leftBoxLayout);
	mainBoxLayout->addWidget(poseDisplayer);
	mainBoxLayout->setAlignment(Qt::AlignRight);

	layout->addItem(mainBoxLayout);
     
    setLayout(layout);

    totalWidth = std::max(sequenceDisplayer->width(), 
                        (infoIndicator->width() + poseDisplayer->width()));
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
	delete icon0;
	icon0 = NULL;
	delete icon1;
	icon1 = NULL;
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
}

void MainGUI::connectSignallerToProfileIcons(GestureSignaller *signaller)
{
	QObject::connect(signaller, SIGNAL(emitToggleActiveIcon()),
		this, SLOT(handleUpdateProfile()));
}

void MainGUI::setupProfileIcons()
{
	QImage icon0Active(QString(PROFILE_ICON0_ACTIVE));
	QImage icon0Inactive(QString(PROFILE_ICON0_INACTIVE));
	QImage icon1Active(QString(PROFILE_ICON1_ACTIVE));
	QImage icon1Inactive(QString(PROFILE_ICON1_INACTIVE));

	icon0IsActive = true;
	icon0 = new ProfileIcon(SPECIFIC_PROFILE_ICON_SIZE, SPECIFIC_PROFILE_ICON_SIZE, true, QPixmap::fromImage(icon0Active), QPixmap::fromImage(icon0Inactive), this);
	icon1 = new ProfileIcon(SPECIFIC_PROFILE_ICON_SIZE, SPECIFIC_PROFILE_ICON_SIZE, false, QPixmap::fromImage(icon1Active), QPixmap::fromImage(icon1Inactive), this);
}

void MainGUI::handleUpdateProfile()
{
	// Currently just toggling active display between 2 choices... quite hard coded, but will remain this way for now.
	if (icon0IsActive)
	{
		icon0->setImgActiveSel(false);
		icon1->setImgActiveSel(true);
		icon0IsActive = false;
	}
	else
	{
		icon0->setImgActiveSel(true);
		icon1->setImgActiveSel(false);
		icon0IsActive = true;
	}
}