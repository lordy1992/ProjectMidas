#include <limits.h>
#include "gtest/gtest.h"
#include "MyoTranslationFilter.h"

/**
* Tests associated with MyoTranslationFilter
*
* Using gtest. Consult https://code.google.com/p/googletest/wiki/AdvancedGuide for questions.
*/

//This should pass.
TEST(MyoTranslationFilterTest1, calcRingDelta) {
    // Name = calcRingDelta
    // Test Class = MyoTranslationFilterTest1
    MyoTranslationFilter *mtf = new MyoTranslationFilter(NULL);

    // JORDEN TODO - input IN RADIANSSSSS!!!

    //Invalid inputs
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(-1.0, 90.0));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(90.0, -1.0));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(-1.0, -1.0));

    //Valid inputs
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(90.0, 90.0));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(0.0, 0.0));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(0.0, 0.0));

    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(90, 90));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(90, 90));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(90, 90));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(90, 90));

}
