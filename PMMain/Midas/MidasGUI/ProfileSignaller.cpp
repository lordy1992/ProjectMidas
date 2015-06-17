#include "ProfileSignaller.h"

ProfileSignaller::ProfileSignaller(QObject *parent)
    : QObject(parent)
{
    profileName = "default";
}

ProfileSignaller::~ProfileSignaller()
{

}

void ProfileSignaller::handleProfilePress(QString name)
{
    profileName = name.toStdString();
}

std::string ProfileSignaller::getProfileName()
{
    return profileName;
}

void ProfileSignaller::setProfileName(std::string name)
{
	profileName = name;
}