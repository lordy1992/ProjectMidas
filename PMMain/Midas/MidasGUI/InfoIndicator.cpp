#include "InfoIndicator.h"
#include <QTimer.h>
#include <QAction.h>
#include <QApplication.h>
#include <QEvent.h>
#include <QPainter.h>
#include <qstyle.h>
#include <qdesktopwidget.h>
#include <math.h>
#include <qmessagebox.h>

InfoIndicator::InfoIndicator(int widgetWidth, int widgetHeight, QWidget *parent)
    : DraggableWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    indWidth(widgetWidth), indHeight(widgetHeight), showAll(false)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);

    // Temporarily allow a Quit
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl-Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the info indicator with the left mouse button.\n"
        "Use the right mouse button to open a context menu."));
    setWindowTitle(tr("Info Indicator"));

    setWindowOpacity(0.75);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    layout = new QGridLayout;
    //layout->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    layout->setSpacing(5);
    setLayout(layout);

    QFont timesFont("Times", 9, QFont::Bold);
    stateLabel = new QLabel(tr("%1").arg("jorden test"));
    stateLabel->setFont(timesFont);
    layout->addWidget(stateLabel, 0, 0);

    button = new QPushButton(this);
    button->setText(getShowAllString());
    QFont timesSmall("Times", 8, QFont::DemiBold);
    button->setFont(timesSmall);
    connect(button, SIGNAL(released()), this, SLOT(handleButton()));
    layout->addWidget(button, 0, 1);

    // Position the widget on the bottom-right initially.
    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - indWidth - WIDGET_BUFFER, screen.bottom() - indHeight - MOUSE_INDICATOR_SIZE - 2*WIDGET_BUFFER,
        indWidth, indHeight);
}

void InfoIndicator::paintEvent(QPaintEvent *event)
{
}

void InfoIndicator::resizeEvent(QResizeEvent *event)
{
    QRegion maskedRegion(0, 0 , width(), height(), QRegion::Rectangle);
    setMask(maskedRegion);
}

void InfoIndicator::handleButton()
{
    showAll = !showAll;
    button->setText(getShowAllString());
    emitShowAllToggle(showAll);
}

QSize InfoIndicator::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void InfoIndicator::handleUpdateState(QString stateString)
{
    stateLabel->setText(stateString);
}

QString InfoIndicator::getShowAllString()
{
    if (showAll)
    {
        return tr("less");
    }
    else
    {
        return tr("more");
    }

}