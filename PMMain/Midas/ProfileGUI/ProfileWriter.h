#ifndef PROFILE_WRITER_H
#define PROFILE_WRITER_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct Command {
    std::string type;
    std::vector<std::string> actions;
};

struct Gesture {
    std::string type;
    std::string name;
};

struct Sequence {
    std::string state;
    std::vector<Gesture> gestures;
    Command cmd;
};

struct Profile {
    std::string profileName;
    std::vector<Sequence> sequences;
};


class ProfileWriter
{
public:
    ProfileWriter();
    ~ProfileWriter();
    
    void writeProfiles(std::string filename, std::vector<Profile> profiles);

private:
    void writeProfile(boost::property_tree::ptree &profileNode, Profile profile);
    void writeSequence(boost::property_tree::ptree &sequenceNode, Sequence sequence);
};

#endif /* PROFILE_WRITER_H */