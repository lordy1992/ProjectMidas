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

    //std::string getProfileName();
	//void setProfileName(std::string name);

public slots:
    void handleProfilePress(QString);

private:
    //std::string profileName;
	ControlState* controlStateHandle;
};

#endif // PROFILESIGNALLER_H
