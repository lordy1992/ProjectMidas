
#include <limits.h>
#include "gtest/gtest.h"
#include "MyoTranslationFilter.h"

//This should pass.
TEST(MyoTranslationFilterTest1, testFuncTest) {
    // Name = testFuncTest
    // Test Class = MyoTranslationFilterTest1
    MyoTranslationFilter *mtf = new MyoTranslationFilter(NULL);

    EXPECT_EQ(1, 1);

    EXPECT_EQ(2, mtf->testFunc(1));
    EXPECT_GT(5, mtf->testFunc(1));
}

//This should fail.
TEST(MyoTranslationFilterTest2, testFuncTest) {
    MyoTranslationFilter *mtf = new MyoTranslationFilter(NULL);

    EXPECT_EQ(1, 1);

    EXPECT_EQ(4, mtf->testFunc(1));
    EXPECT_GT(2, mtf->testFunc(1));
}