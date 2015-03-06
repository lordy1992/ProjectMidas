#include <QtWidgets\QApplication.h>
#include "ProfileWriter.h"

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

    Sequence unlockSequence, leftClickSequence;

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

    std::vector<Profile> profiles;
    Profile basicProfile;
    basicProfile.profileName = "basic";
    basicProfile.sequences.push_back(unlockSequence);
    basicProfile.sequences.push_back(leftClickSequence);

    profiles.push_back(basicProfile);

    ProfileWriter testWriter;

    testWriter.writeProfiles("testProfile.xml", profiles);
    return 0;
}