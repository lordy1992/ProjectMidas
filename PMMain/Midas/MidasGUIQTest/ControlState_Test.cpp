#include "ControlState.h"
#include "SharedCommandData.h"
#include <gtest\gtest.h>

TEST(ControlStateTest1, testConstruct) {
    SharedCommandData *scd = new SharedCommandData();
    ControlState *cs = new ControlState(scd);

    midasMode mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);

    delete scd;
    delete cs;
}

TEST(ControlStateTest2, testSet) {
    SharedCommandData *scd = new SharedCommandData();
    ControlState *cs = new ControlState(scd);

    midasMode mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);

    cs->setMode(midasMode::GESTURE_MODE);
    mm = cs->getMode();
    ASSERT_EQ(midasMode::GESTURE_MODE, mm);

    cs->setMode(midasMode::KEYBOARD_MODE);
    mm = cs->getMode();
    ASSERT_EQ(midasMode::KEYBOARD_MODE, mm);

    cs->setMode(midasMode::MOUSE_MODE);
    mm = cs->getMode();
    ASSERT_EQ(midasMode::MOUSE_MODE, mm);

    delete scd;
    delete cs;
}

TEST(ControlStateTest3, testTrySet) {
    SharedCommandData *scd = new SharedCommandData();
    ControlState *cs = new ControlState(scd);

    midasMode mm = cs->getMode();
    ASSERT_EQ(midasMode::LOCK_MODE, mm);

    cs->trySetMode(midasMode::GESTURE_MODE);
    mm = cs->getMode();
    ASSERT_EQ(midasMode::GESTURE_MODE, mm);

    cs->trySetMode(midasMode::KEYBOARD_MODE);
    mm = cs->getMode();
    ASSERT_EQ(midasMode::KEYBOARD_MODE, mm);

    cs->trySetMode(midasMode::MOUSE_MODE);
    mm = cs->getMode();
    ASSERT_EQ(midasMode::MOUSE_MODE, mm);

    delete scd;
    delete cs;
}
