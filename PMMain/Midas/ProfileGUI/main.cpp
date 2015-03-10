#include <QtWidgets\QApplication.h>
#include "ProfileWriter.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Gesture thumbToPinky, waveIn, waveOut, fist;
    thumbToPinky.name = "ThumbToPinky";
    thumbToPinky.type = "tap";
    waveIn.name = "WaveIn";
    waveIn.type = "tap";
    waveOut.name = "WaveOut";
    waveOut.type = "tap";
    fist.name = "Fist";
    fist.type = "immediate";

    Sequence unlockSequence, leftClickSequence, crazySequence;

    unlockSequence.state = "locked";

    unlockSequence.gestures.push_back(thumbToPinky);
    unlockSequence.gestures.push_back(waveIn);
    unlockSequence.gestures.push_back(waveOut);

    unlockSequence.cmd.type = "stateChange";
    unlockSequence.cmd.actions.push_back("mouseMode");

    leftClickSequence.state = "mouse";
    
    leftClickSequence.gestures.push_back(fist);
    leftClickSequence.cmd.type = "mouse";
    leftClickSequence.cmd.actions.push_back("leftClick");

    crazySequence.state = "gesture";

    crazySequence.gestures.push_back(fist);
    crazySequence.gestures.push_back(waveIn);
    crazySequence.gestures.push_back(waveOut);
    crazySequence.gestures.push_back(thumbToPinky);
    crazySequence.gestures.push_back(fist);

    crazySequence.cmd.type = "keyboard";
    crazySequence.cmd.actions.push_back("CTRL");
    crazySequence.cmd.actions.push_back("+");

    std::vector<Profile> profiles;
    Profile basicProfile;
    basicProfile.profileName = "basic";
    basicProfile.sequences.push_back(unlockSequence);
    basicProfile.sequences.push_back(leftClickSequence);

    profiles.push_back(basicProfile);

    Profile otherProfile;
    otherProfile.profileName = "other";
    otherProfile.sequences.push_back(leftClickSequence);
    otherProfile.sequences.push_back(crazySequence);

    profiles.push_back(otherProfile);

    ProfileWriter testWriter;

    testWriter.writeProfiles("testProfile.xml", profiles);

    std::vector<Profile> testProfiles = testWriter.loadProfilesFromFile("testProfile.xml");

    if (profiles.size() != testProfiles.size())
    {
        std::cout << "The profiles do not match as expected. Sizes do not match." << std::endl;
        return 0;
    }

    for (int i = 0; i < testProfiles.size(); i++)
    {
        if (testProfiles[i].profileName != profiles[i].profileName)
        {
            std::cout << "The profile names do not match." << std::endl;
            std::cout << "Saved profile name: " << profiles[i].profileName << ", Loaded: "
                << testProfiles[i].profileName << std::endl;
            break;
        }

        std::vector<Sequence> loadedSequences = testProfiles[i].sequences;
        std::vector<Sequence> savedSequences = profiles[i].sequences;

        if (loadedSequences.size() != savedSequences.size())
        {
            std::cout << "The sizes of the sequences do not match." << std::endl;
            break;
        }

        for (int j = 0; j < loadedSequences.size(); j++)
        {
            if (loadedSequences[j].state != savedSequences[j].state)
            {
                std::cout << "The states do not mach." << std::endl;
                std::cout << "Saved sequence state: " << savedSequences[j].state << ", Loaded: "
                    << loadedSequences[j].state << std::endl;
                return 0;
            }

            Command loadedCmd = loadedSequences[j].cmd;
            Command savedCmd = loadedSequences[j].cmd;

            if (loadedCmd.type != savedCmd.type)
            {
                std::cout << "The types do not match." << std::endl;
                std::cout << "Saved cmd type: " << savedCmd.type << ", Loaded: "
                    << loadedCmd.type << std::endl;
                return 0;
            }

            if (loadedCmd.actions.size() != savedCmd.actions.size())
            {
                std::cout << "The actions array sizes do not match." << std::endl;
                return 0;
            }

            for (int k = 0; k < loadedCmd.actions.size(); k++)
            {
                if (loadedCmd.actions[k] != savedCmd.actions[k])
                {
                    std::cout << "The actions do not match." << std::endl;
                    std::cout << "Saved action: " << savedCmd.actions[k] << ", Loaded: "
                        << loadedCmd.actions[k] << std::endl;
                    return 0;
                }
            }

            std::vector<Gesture> loadedGestures = loadedSequences[j].gestures;
            std::vector<Gesture> savedGestures = savedSequences[j].gestures;

            if (loadedGestures.size() != savedGestures.size())
            {
                std::cout << "The gestures are not the same size." << std::endl;
                return 0;
            }

            for (int k = 0; k < loadedGestures.size(); k++)
            {
                if (savedGestures[k].name != loadedGestures[k].name)
                {
                    std::cout << "The gesture names do not match." << std::endl;
                    std::cout << "Saved gesture name: " << savedGestures[k].name << ", Loaded: "
                        << loadedGestures[k].name << std::endl;
                    return 0;
                }

                if (savedGestures[k].type != loadedGestures[k].type)
                {
                    std::cout << "The gesture types do not match." << std::endl;
                    std::cout << "Saved gesture type: " << savedGestures[k].type << ", Loaded: "
                        << loadedGestures[k].type << std::endl;
                    return 0;
                }
            }
        }
    }
    return 0;
}