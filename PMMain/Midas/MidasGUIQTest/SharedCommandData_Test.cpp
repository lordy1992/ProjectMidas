#include "SharedCommandData.h"
#include <gtest\gtest.h>

TEST(SharedCommandDataTest1, extractBadCommand)
{
    SharedCommandData scd;
    filterDataMap scdInput;
    scdInput[COMMAND_INPUT] = 5; // Invalid type
    scd.setInput(scdInput);

    scd.process();

    ASSERT_EQ(filterError::INVALID_INPUT, scd.getFilterError());
    ASSERT_EQ(filterStatus::FILTER_ERROR, scd.getFilterStatus());
}

TEST(SharedCommandDataTest2, extractCommand)
{
    SharedCommandData scd;
    filterDataMap scdInput;
    commandData dat;
    dat.type = MOUSE_COMMAND;
    dat.mouse = LEFT_CLICK;

    scdInput[COMMAND_INPUT] = dat;
    scd.setInput(scdInput);

    scd.process();

    ASSERT_EQ(filterError::NO_FILTER_ERROR, scd.getFilterError());
    ASSERT_EQ(filterStatus::END_CHAIN, scd.getFilterStatus());

    commandData outputDat;
    scd.consumeCommand(outputDat);

    ASSERT_EQ(MOUSE_COMMAND, outputDat.type);
    ASSERT_EQ(LEFT_CLICK, outputDat.mouse);
}

TEST(SharedCommandDataTest3, extractVelocity)
{
    SharedCommandData scd;
    filterDataMap scdInput;
    point dat;
    dat.x = 50;
    dat.y = 100;

    scdInput[VELOCITY_INPUT] = dat;
    scd.setInput(scdInput);

    scd.process();

    ASSERT_EQ(filterError::NO_FILTER_ERROR, scd.getFilterError());
    ASSERT_EQ(filterStatus::END_CHAIN, scd.getFilterStatus());

    point outputDat;
    outputDat = scd.getVelocity();

    ASSERT_EQ(50, outputDat.x);
    ASSERT_EQ(100, outputDat.y);
}

TEST(SharedCommandDataTest4, extractBadVelocity)
{
    SharedCommandData scd;
    filterDataMap scdInput;

    scdInput[VELOCITY_INPUT] = 5; // Bad type
    scd.setInput(scdInput);

    scd.process();

    ASSERT_EQ(filterError::INVALID_INPUT, scd.getFilterError());
    ASSERT_EQ(filterStatus::FILTER_ERROR, scd.getFilterStatus());
}