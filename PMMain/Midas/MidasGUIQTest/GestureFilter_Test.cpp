#include "GestureFilter.h"
#include <gtest\gtest.h>
#include <Windows.h>

//TEST(GestureFiltTest1, constructorTest) {
//    SharedCommandData *scd = new SharedCommandData(1);
//    ControlState *cs = new ControlState(scd);
//
//    GestureFilter *gestFilt = new GestureFilter(cs, 0, NULL);
//
//    filterError fErr = gestFilt->getFilterError();
//    ASSERT_EQ(filterError::NO_FILTER_ERROR, fErr);
//
//    delete gestFilt;
//    delete scd;
//    delete cs;
//}
//
//TEST(GestureFiltTest2, timingTest) {
//    SharedCommandData *scd = new SharedCommandData(1);
//    ControlState *cs = new ControlState(scd);
//
//    GestureFilter *gestFilt = new GestureFilter(cs, 20, NULL);
//
//    midasMode mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//
//    // Ensure Mouse Mode can't be entered if user doesnt "hold" a pose for long enough
//    /* This portion of the test is EXTREMELY specific to implementation of GestureFilter.cpp */
//    filterDataMap input;
//
//    input[GESTURE_INPUT] = myo::Pose::Type::reserved1;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // Ensure sequence timeout.
//    Sleep(100);
//
//    // Ensure unlocking works with proper delay.
//    // UNLOCKING 1/3 with no delay
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    Sleep(21);
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // UNLOCKING 2/3 with no delay
//    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    Sleep(21);
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // UNLOCKING 3/3 with no delay
//    input[GESTURE_INPUT] = myo::Pose::Type::waveOut;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    Sleep(21);
//    ASSERT_EQ(midasMode::MOUSE_MODE, mm); // should reached MOUSE_MODE since long enough delay.
//
//    // Ensure sequence timeout.
//    Sleep(100);
//
//    // Ensure timing synchs up and too short of delays do NOT work.
//    // LOCK 1/2 with too short of delay
//    Sleep(19);
//    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    Sleep(19);
//    ASSERT_EQ(midasMode::MOUSE_MODE, mm);
//    // LCOKING 2/2 with too short of delay
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm); // should NOT have reached MOUSE_MODE since not long enough delay.
//
//    delete gestFilt;
//    delete scd;
//    delete cs;
//}
//
//TEST(GestureFiltTest3, timingTest2) {
//    SharedCommandData *scd = new SharedCommandData(1);
//    ControlState *cs = new ControlState(scd);
//
//    GestureFilter *gestFilt = new GestureFilter(cs, 20, NULL);
//
//    midasMode mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//
//    // Ensure Mouse Mode can't be entered if user doesnt "hold" a pose for long enough
//    /* This portion of the test is EXTREMELY specific to implementation of GestureFilter.cpp */
//    filterDataMap input;
//    
//    // UNLOCKING 1/3 with no delay
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // UNLOCKING 2/3 with no delay
//    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // UNLOCKING 3/3 with no delay
//    input[GESTURE_INPUT] = myo::Pose::Type::waveOut;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm); // should NOT have reached MOUSE_MODE since not long enough delay.
//    
//    delete gestFilt;
//    delete scd;
//    delete cs;
//}
//
//TEST(GestureFiltTest4, stateMachineTest) {
//    SharedCommandData *scd = new SharedCommandData(1);
//    ControlState *cs = new ControlState(scd);
//
//    GestureFilter *gestFilt = new GestureFilter(cs, 0, NULL);
//
//    midasMode mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//
//    /* This portion of the test is EXTREMELY specific to implementation of GestureFilter.cpp */
//    filterDataMap input;
//    // UNLOCKING 1/3
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // UNLOCKING 2/3
//    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//    // UNLOCKING 3/3
//    input[GESTURE_INPUT] = myo::Pose::Type::waveOut;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::MOUSE_MODE, mm);
//
//    // GESTUREMODE 1/1
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::GESTURE_MODE, mm);
//    // BACK to MOUSEMODE 1/1
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::MOUSE_MODE, mm);
//
//    // LOCKING 1/2
//    input[GESTURE_INPUT] = myo::Pose::Type::waveIn;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::MOUSE_MODE, mm);
//    // LOCKING 2/2
//    input[GESTURE_INPUT] = myo::Pose::Type::thumbToPinky;
//    gestFilt->setInput(input);
//    gestFilt->process();
//    mm = cs->getMode();
//    ASSERT_EQ(midasMode::LOCK_MODE, mm);
//
//    delete gestFilt;
//    delete scd;
//    delete cs;
//}