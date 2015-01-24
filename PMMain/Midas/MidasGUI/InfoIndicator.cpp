#include "InfoIndicator.h"
#include <QTimer.h>
#include <QAction.h>
#include <QApplication.h>
#include <QEvent.h>
#include <QPainter.h>
#include <qdesktopwidget.h>
#include <math.h>
#include <qmessagebox.h>

InfoIndicator::InfoIndicator(int widgetWidth, int widgetHeight, QWidget *parent)
    : DraggableWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    indWidth(widgetWidth), indHeight(widgetHeight)
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

    QFont timesFont("Times", 9, QFont::Bold);
    QLabel *label = new QLabel(tr("%1").arg("jorden test"));
    label->setFont(timesFont);
    label->setWordWrap(true);
    formBoxLabel(label);
    stateLabel = label;

    // Position the widget on the bottom-right initially.
    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - indWidth - WIDGET_BUFFER, screen.bottom() - indHeight - MOUSE_INDICATOR_SIZE - 2*WIDGET_BUFFER,
        indWidth, indHeight);
}

void InfoIndicator::paintEvent(QPaintEvent *event)
{

    //int side = qMin(width(), height());
    //QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.translate(width() / 2, height() / 2);
    //
    //QColor cursorIndicatorColor(200, 0, 0);
    //QBrush cursorIndicatorBrush(cursorIndicatorColor);
    //QPen cursorIndicatorPen(cursorIndicatorBrush, 5);
    //painter.setPen(cursorIndicatorPen);
    //painter.setBrush(Qt::NoBrush);
    //
    //painter.drawPoint(cursorPos);


    stateLabel->setEnabled(true);
    stateLabel->setFrameShape(QFrame::Box);
    stateLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    stateLabel->setBackgroundRole(QPalette::Base);
    stateLabel->setAlignment(Qt::AlignLeft);
    stateLabel->setAutoFillBackground(true);
    stateLabel->setFixedSize(width(), height());
    stateLabel->setScaledContents(true);
    stateLabel->setHidden(false);
}

void InfoIndicator::resizeEvent(QResizeEvent *event)
{
    QRegion maskedRegion(0, 0 , width(), height(), QRegion::Rectangle);
    setMask(maskedRegion);
}

QSize InfoIndicator::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void InfoIndicator::handleUpdateState(QString stateString)
{
    stateLabel->setText(stateString);
}