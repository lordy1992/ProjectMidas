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

    //std::ofstream profileFile(filename.c_str());
    write_xml(filename, tr, std::locale(), boost::property_tree::xml_writer_make_settings<ptree::key_type>(' ', 4u));
    //profileFile.close();
}

void ProfileWriter::writeProfile(boost::property_tree::ptree &profileNode, Profile profile)
{
    using boost::property_tree::ptree;

    BOOST_FOREACH(Sequence sequence, profile.sequences) 
    {
        ptree &sequenceNode = profileNode.add("sequence", "");
        sequenceNode.put("<xmlattr>.state", sequence.state);
        writeSequence(sequenceNode, sequence);
    }
}

void ProfileWriter::writeSequence(boost::property_tree::ptree &sequenceNode, Sequence sequence)
{
    using boost::property_tree::ptree;

    BOOST_FOREACH(Gesture gesture, sequence.gestures) 
    {
        ptree &gestureNode = sequenceNode.add("gesture", gesture.name);
        gestureNode.put("<xmlattr>.type", gesture.type);
    }

    ptree &cmdNode = sequenceNode.add("command", "");
    cmdNode.put("<xmlattr>.type", sequence.cmd.type);

    BOOST_FOREACH(std::string action, sequence.cmd.actions)
    {
        cmdNode.add("actions.action", action);
    }
}