#ifndef PROFILESIGNALLER_H
#define PROFILESIGNALLER_H

#include "ControlState.h"
#include <QObject>

class ProfileSignaller : public QObject
{
    Q_OBJECT

public:
    ProfileSignaller(QObject *parent = 0);
    ~ProfileSignaller();

	void setControlStateHandle(ControlState* handle) { controlStateHandle = handle; }

public slots:
    void handleProfilePress(QString);

private:
	ControlState* controlStateHandle;
};

#endif // PROFILESIGNALLER_H
