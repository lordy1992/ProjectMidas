#include "RingData.h"


ringData::ringData()
{
    this->ringInVector = new std::vector<keyboard_value>();
    this->ringOutVector = new std::vector<keyboard_value>();
}


ringData::~ringData()
{
    delete this->ringOutVector;
    delete this->ringInVector;
}



std::vector<ringData::keyboard_value> *ringData::getRingInVectorHandle()
{
    return ringInVector;
}

std::vector<ringData::keyboard_value> *ringData::getRingOutVectorHandle()
{
    return ringOutVector;
}

void ringData::setRingInVector(std::vector<keyboard_value> ringInVal)
{
    *ringInVector = ringInVal;
}

void ringData::setRingOutVector(std::vector<keyboard_value> ringOutVal)
{
    *ringOutVector = ringOutVal;
}