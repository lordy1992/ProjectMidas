#include "RingData.h"


ringData::ringData()
{
    this->ringInVector = new std::vector<keyboardValue>();
    this->ringOutVector = new std::vector<keyboardValue>();
}


ringData::~ringData()
{
    delete this->ringOutVector;
    delete this->ringInVector;
}

ringData::ringData(const ringData& rd)
{
    this->ringInVector = new std::vector<keyboardValue>(*rd.ringInVector);
    this->ringOutVector = new std::vector<keyboardValue>(*rd.ringOutVector);
}

std::vector<ringData::keyboardValue> *ringData::getRingInVectorHandle()
{
    return ringInVector;
}

std::vector<ringData::keyboardValue> *ringData::getRingOutVectorHandle()
{
    return ringOutVector;
}

void ringData::setRingInVector(std::vector<keyboardValue> ringInVal)
{
    *ringInVector = ringInVal;
}

void ringData::setRingOutVector(std::vector<keyboardValue> ringOutVal)
{
    *ringOutVector = ringOutVal;
}