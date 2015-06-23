#include <fstream>
#include <iostream>
#include "AveragingFilter.h"
#include "myo\myo.hpp"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

AveragingFilter::AveragingFilter() : avgCount(DEFAULT_AVG_COUNT) {}

AveragingFilter::AveragingFilter(unsigned int avgCount) : avgCount(avgCount) {}

AveragingFilter::~AveragingFilter() {}

void AveragingFilter::setAvgCount(unsigned int avgCount)
{
    this->avgCount = avgCount;
}

unsigned int AveragingFilter::getAvgCount() const
{
    return avgCount;
}

void AveragingFilter::process()
{
    filterDataMap input = Filter::getInput();

    float quatX  = boost::any_cast<float>(input[QUAT_DATA_X ]);
    float quatY  = boost::any_cast<float>(input[QUAT_DATA_Y ]);
    float quatZ  = boost::any_cast<float>(input[QUAT_DATA_Z ]);
    float quatW  = boost::any_cast<float>(input[QUAT_DATA_W ]);
    float accelX = boost::any_cast<float>(input[ACCEL_DATA_X]);
    float accelY = boost::any_cast<float>(input[ACCEL_DATA_Y]);
    float accelZ = boost::any_cast<float>(input[ACCEL_DATA_Z]);
    float gyroX  = boost::any_cast<float>(input[GYRO_DATA_X ]);
    float gyroY  = boost::any_cast<float>(input[GYRO_DATA_Y ]);
    float gyroZ  = boost::any_cast<float>(input[GYRO_DATA_Z ]);
    int8_t rssi  = boost::any_cast<int8_t>(input[RSSI]);

    Arm arm = boost::any_cast<Arm>(input[INPUT_ARM]);
    XDirection xDirection = boost::any_cast<XDirection>(input[INPUT_X_DIRECTION]);

    insertAvgElement(quatX, quatXDeque);    
    insertAvgElement(quatY, quatYDeque);
    insertAvgElement(quatZ, quatZDeque);
    insertAvgElement(quatW, quatWDeque);
    insertAvgElement(accelX,accelXDeque);
    insertAvgElement(accelY,accelYDeque);
    insertAvgElement(accelZ,accelZDeque);
    insertAvgElement(gyroX, gyroXDeque);
    insertAvgElement(gyroY, gyroYDeque);
    insertAvgElement(gyroZ, gyroZDeque);
    insertAvgElement((float)rssi, rssiDeque);

    filterDataMap output;

    output[QUAT_DATA_X]  = calcAvg(quatXDeque);
    output[QUAT_DATA_Y]  = calcAvg(quatYDeque);
    output[QUAT_DATA_Z]  = calcAvg(quatZDeque);
    output[QUAT_DATA_W]  = calcAvg(quatWDeque);
    output[ACCEL_DATA_X] = calcAvg(accelXDeque);
    output[ACCEL_DATA_Y] = calcAvg(accelYDeque);
    output[ACCEL_DATA_Z] = calcAvg(accelZDeque);
    output[GYRO_DATA_X]  = calcAvg(gyroXDeque);
    output[GYRO_DATA_Y]  = calcAvg(gyroYDeque);
    output[GYRO_DATA_Z]  = calcAvg(gyroZDeque);
    output[RSSI] = calcAvg(rssiDeque);
    output[INPUT_ARM] = arm;
    output[INPUT_X_DIRECTION] = xDirection;

    Filter::setOutput(output);
}

void AveragingFilter::insertAvgElement(float elem, std::deque<float>& dq)
{
    dq.push_back(elem);
    while (dq.size() > avgCount) {
        // discard element so that average is only based on
        // avgCount elements.
        dq.pop_front();
    }
}

float AveragingFilter::calcAvg(std::deque<float>& dq)
{
    float sum = 0;
    float denom = dq.size();
    if (denom == 0)
    {
        return 0;
    }
    
    std::deque<float>::iterator it = dq.begin();
    while (it != dq.end())
    {
        sum += *it++;
    }
    return (float)sum / denom;
}

void AveragingFilter::replaceLastElement(float elem, std::deque<float>&dq)
{
    dq.pop_back();
    dq.push_back(elem);
}