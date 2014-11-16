#ifndef _AVERAGING_FILTER_H
#define _AVERAGING_FILTER_H

#include "Filter.h"
#include <queue>

#define DEFAULT_AVG_COUNT 20

class AveragingFilter : public Filter
{
public:
    AveragingFilter();
    AveragingFilter(unsigned int avgCount);
    ~AveragingFilter();

    void process();

    void setAvgCount(unsigned int avgCount);

    unsigned int getAvgCount() const;

private:
    unsigned int avgCount;

    void insertAvgElement(float elem, std::queue<float>q);

    float calcAvg(std::queue<float>q);

    //Arrays to store the avg data.
    std::queue<float> quatXQueue;
    std::queue<float> quatYQueue;
    std::queue<float> quatZQueue;
    std::queue<float> quatWQueue;
    std::queue<float> accelXQueue;
    std::queue<float> accelYQueue;
    std::queue<float> accelZQueue;
    std::queue<float> gyroXQueue;
    std::queue<float> gyroYQueue;
    std::queue<float> gyroZQueue;
};

#endif /* _AVERAGING_FILTER_H */