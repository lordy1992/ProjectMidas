#include "MainGUI.h"
#include <QApplication.h>
#include <QDesktopWidget.h>
#include <algorithm>

#define SCREEN_RIGHT_BUFFER    20 
#define SCREEN_BOTTOM_BUFFER   30

MainGUI::MainGUI(MidasThread *mainThread, int deadZoneRad)
    : DraggableWidget(NULL, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
    mouseIndicator = new MouseIndicator(mainThread, deadZoneRad, MOUSE_INDICATOR_SIZE,
        MOUSE_INDICATOR_SIZE, this);
    infoIndicator = new InfoIndicator(INFO_INDICATOR_WIDTH, 
        INFO_INDICATOR_HEIGHT, this);
    sequenceDisplayer = new SequenceDisplayer(this);
    poseDisplayer = new PoseDisplayer(GRID_ELEMENT_SIZE, GRID_ELEMENT_SIZE, this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

    layout = new QVBoxLayout;
    QGridLayout *gridLayout = new QGridLayout;

    layout->addWidget(sequenceDisplayer);
    layout->addWidget(infoIndicator);

    gridLayout->addWidget(poseDisplayer, 0, 0, Qt::AlignRight);
    gridLayout->addWidget(mouseIndicator, 0, 1, Qt::AlignRight);
    layout->addLayout(gridLayout);
    //layout->addWidget(mouseIndicator);
    //layout->addWidget(poseDisplayer);

    //layout->setAlignment(mouseIndicator, Qt::AlignRight);
    //layout->setAlignment(poseDisplayer, Qt::AlignLeft);
    layout->setAlignment(infoIndicator, Qt::AlignRight);
    layout->setStretchFactor(infoIndicator, 0);
    
    setLayout(layout);

    int totalWidth = std::max(sequenceDisplayer->width(), 
                        std::max(infoIndicator->width(), mouseIndicator->width()));
    int totalHeight = sequenceDisplayer->height() + infoIndicator->height() + mouseIndicator->height();

    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - totalWidth - SCREEN_RIGHT_BUFFER, screen.bottom() - totalHeight - SCREEN_BOTTOM_BUFFER,
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
    QObject::connect(signaller, SIGNAL(emitPoseEnum(QString)),
        poseDisplayer, SLOT(handlePoseEnum(QString)));
}