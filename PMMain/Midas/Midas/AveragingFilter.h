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

/*
 * Consult Filter.h for concepts regarding Filters.
 * 
 * The purpose of this filter is to take Accelerometer,
 * Gyroscope, and Quaternion data as input, and average
 * said data over a set number of iterations. This will 
 * flush out any spikes.
 *
 * TODO: 1) Determine proper values of avgCount for good usability
 * 2) As soon as Many to one filters are feasible, change this filter 
 * to only filter one float value, and then implement as many as needed.
 */
class AveragingFilter : public Filter
{
public:
    /* Basic Construction/Destruction */
    AveragingFilter();
    AveragingFilter(unsigned int avgCount);
    ~AveragingFilter();

    /* 
     * Implement pure virtual func.
     * Purpose: 
     *      Access data from Inputs, average the data
     * based on avgCount, and current state, then
     * output the averaged value.
     */
    void process();

    /* Setter */
    void setAvgCount(unsigned int avgCount);

    /* Accessor */
    unsigned int getAvgCount() const;

private:
    /*
     * Input:
     *      elem - a float to be added to deque
     *      dq -  a deque containing <= avgCount floats
     * Purpose:
     *      Inject elem into dq and if necessary, pop 
     *      front of dq to ensure only max avgCount elements
     */
    void insertAvgElement(float elem, std::deque<float>& dq);

    /*
     * Input:
     *      dq - a deque containing <= avgCount floats
     * Output:
     *      float - the average of the contents of the deque            
     * Purpose:
     *      calculate the average of the contents in the deque.
     */
    float calcAvg(std::deque<float>& dq);

    // Number of floats to average. This value dictates max size of deques.
    unsigned int avgCount;

    //Arrays to store the avg data. -- TODO - when we get Filter
    //  Many-to-one working, genericize this to just filtering one float
    // Deques to hold sensor data
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