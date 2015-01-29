#include "MainGUI.h"
#include <QApplication.h>
#include <QDesktopWidget.h>
#include <algorithm>

#define SCREEN_RIGHT_BUFFER 20 

MainGUI::MainGUI(MidasThread *mainThread, int deadZoneRad)
    : DraggableWidget(NULL, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
    mouseIndicator = new MouseIndicator(mainThread, deadZoneRad, 100, 100, this);
    infoIndicator = new InfoIndicator(250, 50, this);
    sequenceDisplayer = new SequenceDisplayer(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

    layout = new QVBoxLayout;

    layout->addWidget(sequenceDisplayer);
    layout->addWidget(infoIndicator);
    layout->addWidget(mouseIndicator);

    layout->setAlignment(mouseIndicator, Qt::AlignRight);
    layout->setAlignment(infoIndicator, Qt::AlignRight);
    layout->setStretchFactor(infoIndicator, 0);
    
    setLayout(layout);

    int totalWidth = std::max(sequenceDisplayer->width(), std::max(infoIndicator->width(), mouseIndicator->width()));
    int totalHeight = sequenceDisplayer->height() + infoIndicator->height() + mouseIndicator->height();

    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - totalWidth - SCREEN_RIGHT_BUFFER, screen.bottom() - totalHeight,
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
    delete layout;
    layout = NULL;
}

MouseIndicator *MainGUI::getMouseIndicator()
{
    return mouseIndicator;
}

InfoIndicator *MainGUI::getInfoIndicator()
{
    return infoIndicator;
}

SequenceDisplayer *MainGUI::getSequenceDisplayer()
{
    return sequenceDisplayer;
}