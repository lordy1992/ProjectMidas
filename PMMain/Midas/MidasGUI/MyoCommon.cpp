
#include "MyoCommon.h"


std::string PoseTypeToString(Pose::Type gesture)
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
    else if (gesture == Pose::Type::doubleTap)
    {
        return "doubleTap";
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
    else
    {
        return "ErrorInMyoCommonToString";
    }
}
