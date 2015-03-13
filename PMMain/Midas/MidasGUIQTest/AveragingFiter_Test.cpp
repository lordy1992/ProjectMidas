#include "AveragingFilter.h"
#include <gtest\gtest.h>
#include <myo\myo.hpp>

TEST(AverageFilterTest1, constructor) {
    AveragingFilter *af = new AveragingFilter();

    ASSERT_EQ(DEFAULT_AVG_COUNT, af->getAvgCount());

    delete af;
}

TEST(AverageFilterTest2, processBasics) {
    AveragingFilter *af = new AveragingFilter();

    int avgCount = af->getAvgCount();
    double tolerance = 1e-6;

    filterDataMap input;

    float val = 1.0;

    // simply test with one value.
    input[QUAT_DATA_X] = val;
    input[QUAT_DATA_X] = val;
    input[QUAT_DATA_Y] = val;
    input[QUAT_DATA_Z] = val;
    input[QUAT_DATA_W] = val;
    input[ACCEL_DATA_X]= val;
    input[ACCEL_DATA_Y]= val;
    input[ACCEL_DATA_Z]= val;
    input[GYRO_DATA_X] = val;
    input[GYRO_DATA_Y] = val;
    input[GYRO_DATA_Z] = val;
    input[INPUT_ARM] = myo::Arm::armRight;
    input[INPUT_X_DIRECTION] = myo::XDirection::xDirectionTowardWrist;

    af->setInput(input);
    
    float sum = 0.0;
    for (int i = 1; i <= avgCount; i++)
    {
        af->process();
        filterDataMap output = af->getOutput();
        ASSERT_NEAR(val, boost::any_cast<float>(output[QUAT_DATA_X]), tolerance);
    }
    
    delete af;
}

TEST(AverageFilterTest3, processAdvanced) {
    AveragingFilter *af = new AveragingFilter();

    int avgCount = af->getAvgCount();
    double tolerance = 1e-6;

    filterDataMap input;

    float val = 1.0;

    // simply test with one value.
    input[QUAT_DATA_X] = val;
    input[QUAT_DATA_X] = val;
    input[QUAT_DATA_Y] = val;
    input[QUAT_DATA_Z] = val;
    input[QUAT_DATA_W] = val;
    input[ACCEL_DATA_X] = val;
    input[ACCEL_DATA_Y] = val;
    input[ACCEL_DATA_Z] = val;
    input[GYRO_DATA_X] = val;
    input[GYRO_DATA_Y] = val;
    input[GYRO_DATA_Z] = val;
    input[INPUT_ARM] = myo::Arm::armRight;
    input[INPUT_X_DIRECTION] = myo::XDirection::xDirectionTowardWrist;

    af->setInput(input);

    float sum = 0.0;
    for (int i = 1; i <= avgCount; i++)
    {
        input[QUAT_DATA_X] = static_cast<float>(i);
        af->setInput(input);
        sum += i;
        af->process();
        filterDataMap output = af->getOutput();
        ASSERT_NEAR(sum / i, boost::any_cast<float>(output[QUAT_DATA_X]), tolerance);
    }

    delete af;
}