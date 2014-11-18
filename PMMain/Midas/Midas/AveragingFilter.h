#ifndef _AVERAGING_FILTER_H
#define _AVERAGING_FILTER_H

#include "Filter.h"
#include <deque>

#define DEFAULT_AVG_COUNT 20
#define QUAT_DATA_X "quatDataX"
#define QUAT_DATA_Y "quatDataY"
#define QUAT_DATA_Z "quatDataZ"
#define QUAT_DATA_W "quatDataW"
#define ACCEL_DATA_X "accelDataX"
#define ACCEL_DATA_Y "accelDataY"
#define ACCEL_DATA_Z "accelDataZ"
#define GYRO_DATA_X "gyroDataX"
#define GYRO_DATA_Y "gyroDataY"
#define GYRO_DATA_Z "gyroDataZ"

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
    void insertAvgElement(float elem, std::deque<float>dq);

    float calcAvg(std::deque<float>dq);

    unsigned int avgCount;

    //Arrays to store the avg data. -- TODO - when we get Filter
    //  Many-to-one working, genericize this to just filtering one float
    std::deque<float> quatXDeque;
    std::deque<float> quatYDeque;
    std::deque<float> quatZDeque;
    std::deque<float> quatWDeque;
    std::deque<float> accelXDeque;
    std::deque<float> accelYDeque;
    std::deque<float> accelZDeque;
    std::deque<float> gyroXDeque;
    std::deque<float> gyroYDeque;
    std::deque<float> gyroZDeque;
};

#endif /* _AVERAGING_FILTER_H */