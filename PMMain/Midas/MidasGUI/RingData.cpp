#include "RingData.h"


ringData::ringData()
{
}


ringData::~ringData()
{
}

std::vector<ringData::keyboard_value> ringData::getRingInVector()
{
    return*ringInVector;
}

std::vector<ringData::keyboard_value> ringData::getRingOutVector()
{
    return*ringOutVector;
}

void ringData::setRingInVector(std::vector<keyboard_value> &ringInVal)
{
    ringInVector = &ringInVal;
}

void ringData::setRingOutVector(std::vector<keyboard_value> &ringOutVal)
{
    ringOutVector = &ringOutVal;
}