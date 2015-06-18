#include "ProfileIcon.h"

ProfileIcon::ProfileIcon(int widgetWidth, int widgetHeight, bool active, QPixmap imgActive, QPixmap imgInactive, QWidget *parent)
	: QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight), imgActive(imgActive), imgInactive(imgInactive)
{
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setToolTip(tr("Drag the profile icon with the left mouse button.\n"
		"Use the right mouse button to open a context menu."));
	setWindowTitle(tr("Profile Icon"));

	setWindowOpacity(0.75);
	QPalette pal;
	pal.setColor(QPalette::Background, QColor(205, 205, 193));
	setAutoFillBackground(true);
	setPalette(pal);
	setWindowFlags(Qt::WindowStaysOnTopHint);

	layout = new QHBoxLayout;
	setLayout(layout);

	// init inactive
	iconLabel = new QLabel();
	if (active)
	{
		iconLabel->setPixmap(imgActive.scaled(indWidth, indHeight));
	}
	else
	{
		iconLabel->setPixmap(imgInactive.scaled(indWidth, indHeight));
	}

	iconLabel->setFrameShape(QFrame::Box);
	iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	iconLabel->setBackgroundRole(QPalette::Base);
	iconLabel->setAutoFillBackground(true);
	iconLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	iconLabel->setScaledContents(true);

	layout->addWidget(iconLabel);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFixedSize(indWidth, indHeight);
}

void ProfileIcon::resizeEvent(QResizeEvent *event)
{
	QRegion maskedRegion(0, 0, width(), height(), QRegion::Rectangle);
	setMask(maskedRegion);
}

QSize ProfileIcon::sizeHint() const
{
	return QSize(indWidth, indHeight);
}

void ProfileIcon::setImgActiveSel(bool active)
{
	if (active)
	{
		iconLabel->setPixmap(imgActive.scaled(indWidth, indHeight));
	}
	else
	{
		iconLabel->setPixmap(imgInactive.scaled(indWidth, indHeight));
	}
}
