#ifndef PROFILE_ICON_H
#define PROFILE_ICON_H

#include "DraggableWidget.h"
#include "MidasCommon.h"
#include <qlayout.h>
#include <qpixmap.h>

class ProfileIcon : public QWidget
{
	Q_OBJECT

public:
	ProfileIcon(int widgetWidth = SPECIFIC_PROFILE_ICON_SIZE, int widgetHeight = SPECIFIC_PROFILE_ICON_SIZE, bool active = false,
		QPixmap imgActive = QPixmap(), QPixmap imgInactive = QPixmap(), QWidget *parent = 0);
	QSize sizeHint() const;

	void setImgActiveSel(bool active);
		
protected:
	void resizeEvent(QResizeEvent *event);

private:
	QPoint position;
	int indWidth, indHeight;
	QHBoxLayout *layout;
	QLabel *iconLabel;
	QPixmap imgActive;
	QPixmap imgInactive;

	bool imgActiveSel;
};

#endif PROFILE_ICON_H