#include "ProfileDisplayer.h"
#include <QLabel.h>
#include <QEvent.h>

ProfileDisplayer::ProfileDisplayer(std::string name, int widgetWidth, int widgetHeight, QWidget *parent)
    : QFrame(parent), indWidth(widgetWidth), indHeight(widgetHeight), profileName(name)
{
    setWindowOpacity(0.75);
  //  setStyleSheet(QString("background-color: rgba(205, 205, 193, 191); \
   //                       border: 1px solid red; min-width: %1; min-height: %2").arg(QString::number(indWidth),  QString::number(indHeight)));
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setFrameShape(QFrame::Shape::Box);
    setFrameShadow(QFrame::Shadow::Raised);

    layout = new QGridLayout;
    layout->setSpacing(5);
    setLayout(layout);

    QFont timesFont("Times", 9, QFont::Bold);
    QLabel* poseImgLabel = new QLabel(QString(name.c_str()));
    poseImgLabel->setFont(timesFont);
   /* poseImgLabel->setFrameShape(QFrame::Box);
    poseImgLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    poseImgLabel->setBackgroundRole(QPalette::Base);
    poseImgLabel->setAutoFillBackground(true);
    poseImgLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    poseImgLabel->setScaledContents(true);*/

    layout->addWidget(poseImgLabel, 0, 0, Qt::AlignLeft);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMinimumSize(indWidth, indHeight);
}

ProfileDisplayer::~ProfileDisplayer()
{
}

QSize ProfileDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void ProfileDisplayer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setFrameShadow(QFrame::Shadow::Sunken);
        event->accept();
    }
}

void ProfileDisplayer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setFrameShadow(QFrame::Shadow::Raised);
        emitChangeProfile(QString(profileName.c_str()));
        event->accept();
    }
}