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

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("sequences")) 
    {
        if (vt.first == "sequence")
        {
            std::string sequenceStateBegin = vt.second.get<std::string>("<xmlattr>.state");
            std::string sequenceName = vt.second.get<std::string>("<xmlattr>.name");
            pr.profileSequences.push_back(extractSequenceInformation(vt, sequenceStateBegin, sequenceName));
        }
    }

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("holds"))
    {
        if (vt.first == "hold")
        {
            hold currHold;
            std::string gesture = vt.second.get<std::string>("<xmlattr>.gesture");
            currHold.gesture = gesture;

            BOOST_FOREACH(const ptree::value_type & angleVt, vt.second)
            {
                if (angleVt.first == "angle")
                {
                    angleAction currAngle;
                    std::string angleType = angleVt.second.get<std::string>("<xmlattr>.type");
                    std::string anglePositive = angleVt.second.get_child("anglePositive").get_value<std::string>();
                    std::string angleNegative = angleVt.second.get_child("angleNegative").get_value<std::string>();
                    currAngle.anglePositive = anglePositive;
                    currAngle.angleNegative = angleNegative;
                    currAngle.type = angleType;
                    currHold.angles.push_back(currAngle);
                }
            }

            pr.holds.push_back(currHold);
        }
    }

    return pr;
}

profileSequence ProfileManager::extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
    std::string sequenceState, std::string sequenceName)
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
    seq.name = sequenceName;

    return seq;
}