
#include "MyoCommon.h"


MyoCommon::MyoCommon()
{
}


MyoCommon::~MyoCommon()
{
}

std::string MyoCommon::PoseTypeToString(Pose::Type gesture)
{
    if (gesture == Pose::Type::fingersSpread)
    {
        return "fingersSpread";
    }
    else if (gesture == Pose::Type::fist)
    {
        return "fist";
    }
    else if (gesture == Pose::Type::rest)
    {
        return "rest";
    }
    else if (gesture == Pose::Type::thumbToPinky)
    {
        return "thumbToPinky";
    }
    else if (gesture == Pose::Type::waveIn)
    {
        return "waveIn";
    }
    else if (gesture == Pose::Type::waveOut)
    {
        return "waveOut";
    }
    else if (gesture == Pose::Type::unknown)
    {
        return "unknown";
    }
    else if (gesture == Pose::Type::reserved1)
    {
        return "reserved1";
    }
    else
    {
        return "ErrorInMyoCommonToString";
    }
}
