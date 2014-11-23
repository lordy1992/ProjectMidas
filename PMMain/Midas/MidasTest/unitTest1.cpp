
#include <limits.h>
#include "gtest/gtest.h"
#include "MyoTranslationFilter.h"

TEST(MyoTranslationFilterTest, testFuncTest) {
    //MyoTranslationFilter *mtf = new MyoTranslationFilter(NULL);

    EXPECT_EQ(1, 1);

    EXPECT_EQ(2, testFunc(1));
    //EXPECT_GT(5, testFunc(1));
}