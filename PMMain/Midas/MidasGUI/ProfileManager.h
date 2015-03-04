#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <string>
#include <boost/property_tree/ptree.hpp>

class ProfileManager {

public:
    ProfileManager();
    ~ProfileManager();

    void loadProfilesFromFile(std::string fileName);

private:
    void extractProfileInformation(boost::property_tree::ptree::value_type const & vt, std::string profileName);
};

#endif /* PROFILE_MANAGER_H */