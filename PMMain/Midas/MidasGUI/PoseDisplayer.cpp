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
    setWindowFlags(Qt::WindowStaysOnTopHint);

    layout = new QGridLayout;
    layout->setSpacing(0);
    setLayout(layout);

    poseImgLabel = new QLabel();
    poseImgLabel->setFrameShape(QFrame::Box);
    poseImgLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    poseImgLabel->setBackgroundRole(QPalette::Base);
    poseImgLabel->setAutoFillBackground(true);
    poseImgLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    poseImgLabel->setScaledContents(true);

    layout->addWidget(poseImgLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(indWidth, indHeight);
}

PoseDisplayer::~PoseDisplayer()
{
    delete poseImgLabel; poseImgLabel = NULL;
    delete layout; layout = NULL;
}

void PoseDisplayer::resizeEvent(QResizeEvent *event)
{
    // todo - could handle resize.
}

QSize PoseDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void PoseDisplayer::handlePoseImages(std::vector<sequenceImageSet> poseImages)
{
    if (poseImages.size() == 1)
    {
        QPixmap scaledPic = poseImages[0].nextImage;
        scaledPic.scaled(indWidth, indHeight);
        poseImgLabel->setPixmap(scaledPic);
    }
}
