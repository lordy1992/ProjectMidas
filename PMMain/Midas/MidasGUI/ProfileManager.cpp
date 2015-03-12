#include "ProfileManager.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <fstream>

ProfileManager::ProfileManager() { }

ProfileManager::~ProfileManager() { }

void ProfileManager::loadProfilesFromFile(std::string fileName)
{
    using boost::property_tree::ptree;

    profiles.clear();

    std::ifstream profileFile(fileName.c_str());

    ptree tr;
    read_xml(profileFile, tr);

    BOOST_FOREACH(const ptree::value_type & vt, tr.get_child("profiles")) {
        if (vt.first == "profile")
        {
            std::string profileName = vt.second.get<std::string>("<xmlattr>.name");
            profiles.push_back(extractProfileInformation(vt, profileName));
        }
    }

    profileFile.close();
}

std::vector<profile>* ProfileManager::getProfiles()
{
    return &profiles;
}

profile ProfileManager::extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName)
{
    using boost::property_tree::ptree;

    profile pr; 
    pr.profileName = profileName;

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second) {
        if (vt.first == "sequence")
        {
            std::string sequenceStateBegin = vt.second.get<std::string>("<xmlattr>.state");
            pr.profileSequences.push_back(extractSequenceInformation(vt, sequenceStateBegin));
        }
    }

    return pr;
}

profileSequence ProfileManager::extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, std::string sequenceState)
{
    using boost::property_tree::ptree;

    profileSequence seq;
    std::vector<gesture> gestures;

    BOOST_FOREACH(const ptree::value_type & vt, parentSequence.second.get_child("gestures")) {
        if (vt.first == "gesture")
        {
            gesture gest;
            gest.type = vt.second.get<std::string>("<xmlattr>.type");
            gest.name = vt.second.get_value<std::string>();
            
            gestures.push_back(gest);
        }
    }

    command cmd;
    const ptree & pt = parentSequence.second.get_child("command");
    cmd.type = pt.get<std::string>("<xmlattr>.type");

    BOOST_FOREACH(const ptree::value_type & vt, pt.get_child("actions")) {
        if (vt.first == "action")
        {
            cmd.actions.push_back(vt.second.get_value<std::string>());
        }
    }

    seq.cmd = cmd;
    seq.gestures = gestures;
    seq.state = sequenceState;

    return seq;
}