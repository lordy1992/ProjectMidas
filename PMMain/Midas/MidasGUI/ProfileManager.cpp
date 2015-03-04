#include "ProfileManager.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <fstream>

ProfileManager::ProfileManager() { }

ProfileManager::~ProfileManager() { }

void ProfileManager::loadProfilesFromFile(std::string fileName)
{
    using boost::property_tree::ptree;

    std::ifstream profileFile(fileName.c_str());

    ptree tr;
    read_xml(profileFile, tr);

    BOOST_FOREACH( ptree::value_type const & vt, tr.get_child("profiles") ) {
        if (vt.first == "profile")
        {
            std::string profileName = vt.second.get<std::string>("<xmlattr>.name");
            extractProfileInformation(vt, profileName);
        }
    }

    profileFile.close();
}

void ProfileManager::extractProfileInformation(boost::property_tree::ptree::value_type const & vt, std::string profileName)
{

}