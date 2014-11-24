
#include "gtest/gtest.h"

//This should pass.
TEST(test1, testFuncTest) {
    // Name = testFuncTest
    // Test Class = MyoTranslationFilterTest1
    EXPECT_EQ(1, 1);
}

//This should fail.
TEST(test2, testFuncTest) {
    EXPECT_EQ(1, 2);
}