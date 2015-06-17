#ifndef PROFILESIGNALLER_H
#define PROFILESIGNALLER_H

#include <QObject>

class ProfileSignaller : public QObject
{
    Q_OBJECT

public:
    ProfileSignaller(QObject *parent = 0);
    ~ProfileSignaller();

    std::string getProfileName();
	void setProfileName(std::string name);

public slots:
    void handleProfilePress(QString);

private:
    std::string profileName;
};

#endif // PROFILESIGNALLER_H
