#include "GestureFilter.h"
#include <gtest\gtest.h>

TEST(GestureFiltTest1, timingTest) {

}


TEST(GestureFiltTest2, stateMachineTest) {
    SharedCommandData *scd = new SharedCommandData();
    ControlState *cs = new ControlState(scd);

    GestureFilter *gestFilt = new GestureFilter(cs, 0);

    filterError fErr = gestFilt->getFilterError();
    ASSERT_EQ(filterError::NO_FILTER_ERROR, fErr);

    midasMode mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);

    /* This portion of the test is EXTREMELY specific to implementation of GestureFilter.cpp */
    filterDataMap input;
    // UNLOCKING 1/3
    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);
    // UNLOCKING 2/3
    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);
    // UNLOCKING 3/3
    input[GESTURE_INPUT] = myo::Pose::Type::waveOut;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::MOUSE_MODE, mm);

    // GESTUREMODE 1/1
    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::GESTURE_MODE, mm);
    // BACK to MOUSEMODE 1/1
    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::MOUSE_MODE, mm);

    // LOCKING 1/2
    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::MOUSE_MODE, mm);
    // LOCKING 2/2
    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
    gestFilt->setInput(input);
    gestFilt->process();
    mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);

    delete gestFilt;
    delete scd;
    delete cs;
}