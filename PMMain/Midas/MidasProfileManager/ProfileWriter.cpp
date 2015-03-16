#include "ProfileWriter.h"
#include <fstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

ProfileWriter::ProfileWriter()
{
}


ProfileWriter::~ProfileWriter()
{
}

void ProfileWriter::writeProfiles(std::string filename, std::vector<Profile> profiles)
{
    using boost::property_tree::ptree;

    ptree tr;

    BOOST_FOREACH(Profile profile, profiles)
    {
        ptree &profileNode = tr.add("profiles.profile", "");
        profileNode.put("<xmlattr>.name", profile.profileName);
        writeProfile(profileNode, profile);
    }

    write_xml(filename, tr, std::locale(), boost::property_tree::xml_writer_make_settings<ptree::key_type>(' ', 4u));
}

void ProfileWriter::writeProfile(boost::property_tree::ptree &profileNode, Profile profile)
{
    using boost::property_tree::ptree;

    BOOST_FOREACH(Sequence sequence, profile.sequences)
    {
        ptree &sequenceNode = profileNode.add("sequence", "");
        sequenceNode.put("<xmlattr>.state", sequence.state);
        sequenceNode.put("<xmlattr>.name", sequence.name);
        writeSequence(sequenceNode, sequence);
    }
}

void ProfileWriter::writeSequence(boost::property_tree::ptree &sequenceNode, Sequence sequence)
{
    using boost::property_tree::ptree;

    BOOST_FOREACH(Gesture gesture, sequence.gestures)
    {
        ptree &gestureNode = sequenceNode.add("gestures.gesture", gesture.name);
        gestureNode.put("<xmlattr>.type", gesture.type);
    }

    ptree &cmdNode = sequenceNode.add("command", "");
    cmdNode.put("<xmlattr>.type", sequence.cmd.type);

    BOOST_FOREACH(std::string action, sequence.cmd.actions)
    {
        cmdNode.add("actions.action", action);
    }
}

std::vector<Profile> ProfileWriter::loadProfilesFromFile(std::string fileName)
{
    using boost::property_tree::ptree;

    std::vector<Profile> profiles;

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

    return profiles;
}

Profile ProfileWriter::extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName)
{
    using boost::property_tree::ptree;

    Profile pr;
    pr.profileName = profileName;

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second) {
        if (vt.first == "sequence")
        {
            std::string sequenceStateBegin = vt.second.get<std::string>("<xmlattr>.state");
            std::string sequenceName = vt.second.get<std::string>("<xmlattr>.name");
            pr.sequences.push_back(extractSequenceInformation(vt, sequenceStateBegin, sequenceName));
        }
    }

    return pr;
}

Sequence ProfileWriter::extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence,
    std::string sequenceState, std::string sequenceName)
{
    using boost::property_tree::ptree;

    Sequence seq;
    std::vector<Gesture> gestures;

    BOOST_FOREACH(const ptree::value_type & vt, parentSequence.second.get_child("gestures")) {
        if (vt.first == "gesture")
        {
            Gesture gest;
            gest.type = vt.second.get<std::string>("<xmlattr>.type");
            gest.name = vt.second.get_value<std::string>();

            gestures.push_back(gest);
        }
    }

    Command cmd;
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