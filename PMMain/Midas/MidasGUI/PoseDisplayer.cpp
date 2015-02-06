#include "PoseDisplayer.h"
#include <QTimer.h>
#include <QAction.h>
#include <QApplication.h>
#include <QEvent.h>
#include <QPainter.h>
#include <qstyle.h>
#include <qdesktopwidget.h>
#include <math.h>
#include <qmessagebox.h>

PoseDisplayer::PoseDisplayer(int widgetWidth, int widgetHeight, QWidget *parent)
    : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight)
{
    // Temporarily allow a Quit
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl-Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the Pose Displayer with the left mouse button.\n"
        "Use the right mouse button to open a context menu."));
    setWindowTitle(tr("Pose Displayer"));

    setWindowOpacity(0.75);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    layout = new QGridLayout;
    layout->setSpacing(5);
    setLayout(layout);

    QFont timesFont("Times", 9, QFont::Bold);
    poseImgLabel = new QLabel(tr("%1").arg("temp")); // todo TEMP!
    poseImgLabel->setFont(timesFont);

    poseImgLabel->setFrameShape(QFrame::Box);
    poseImgLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    poseImgLabel->setBackgroundRole(QPalette::Base);
    poseImgLabel->setAlignment(Qt::AlignCenter);
    poseImgLabel->setAutoFillBackground(true);
    //label->setFixedSize(GRID_ELEMENT_SIZE, GRID_ELEMENT_SIZE);
    poseImgLabel->setScaledContents(true);


    layout->addWidget(poseImgLabel, 0, 0);
    layout->setAlignment(poseImgLabel, Qt::AlignCenter);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMinimumSize(indWidth, indHeight);
}

PoseDisplayer::~PoseDisplayer()
{
    delete this->poseImgLabel;
}

void PoseDisplayer::resizeEvent(QResizeEvent *event)
{
    QRegion maskedRegion(0, 0, width(), height(), QRegion::Rectangle);
    setMask(maskedRegion);
}

QSize PoseDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void PoseDisplayer::handlePoseImages(std::vector<sequenceImageSet> poseImages)
{
    //poseImgLabel->setText(poseEnum); // todo remove
    if (poseImages.size() == 1)
    {
        poseImgLabel->setPixmap(poseImages[0].nextImage);
    }
}

void PoseDisplayer::initPoseImgMap()
{
    //poseImgMap[Pose::rest] = // likely un-needed!
}