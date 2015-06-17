#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "MyoCommon.h"
#include "MidasCommon.h"
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
    std::vector<command> cmds;
};

struct angleAction {
    std::string type;
    std::string anglePositive;
    std::string angleNegative;
};

struct hold {
    std::string gesture;
    std::vector<angleAction> angles;
};

struct profile {
    std::string profileName;
    std::vector<profileSequence> profileSequences;
    std::vector<hold> holds;
};

extern std::map<std::string, commandType> profileCommandToCommandTypeMap;
extern std::map<std::string, kybdGUICmds> profileActionToKybdGui;
extern std::map<std::string, kybdCmds> profileActionToKybd;
extern std::map<std::string, mouseCmds> profileActionToMouseCommands;
extern std::map<std::string, midasMode> profileActionToStateChange;
extern std::map<std::string, profileCmds> profileActionToProfileChange;
extern std::map <std::string, SeqElement::PoseLength> profileGestureTypeToPoseLength;
extern std::map <std::string, Pose::Type> profileGestureNameToType;

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