#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct command {
    std::string type;
    std::vector<std::string> actions;
};

struct gesture {
    std::string type;
    std::string name;
};

struct profileSequence {
    std::string state;
    std::string name;
    std::vector<gesture> gestures;
    command cmd;
};

struct profile {
    std::string profileName;
    std::vector<profileSequence> profileSequences;
};

class ProfileManager {

public:
    ProfileManager();
    ~ProfileManager();

    void loadProfilesFromFile(std::string fileName);

    std::vector<profile>* getProfiles();

private:
    profile extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName);

    profileSequence extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
        std::string sequenceState, std::string sequenceName);

    std::vector<profile> profiles;
};

#endif /* PROFILE_MANAGER_H */