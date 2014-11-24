#include <limits.h>
#include "gtest/gtest.h"
#include "MyoTranslationFilter.h"
#include <math.h>

/**
* Tests associated with MyoTranslationFilter
*
* Using gtest. Consult https://code.google.com/p/googletest/wiki/AdvancedGuide for questions.
*/

/** 
* Helper to convert between degrees/radians
*/
float degToRad(float deg)
{
    return (deg * (M_PI / 180.0));
}

TEST(MyoTranslationFilterTest1, calcRingDeltaInvalid) {
    // Name = calcRingDeltaInvalid
    // Test Class = MyoTranslationFilterTest1
    MyoTranslationFilter *mtf = new MyoTranslationFilter(NULL);

    //Invalid inputs - default return 0.
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(-1.0), degToRad(90.0)));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(90.0), degToRad(-1.0)));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(-1.0), degToRad(-1.0)));

    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(361.0), degToRad(90.0)));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(90.0), degToRad(361.0)));
    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(361.0), degToRad(361.0)));

    EXPECT_FLOAT_EQ(0.0, mtf->calcRingDelta(degToRad(360.0), degToRad(180.0)));

    delete mtf;
}

TEST(MyoTranslationFilterTest2, calcRingDeltaValid) {
    MyoTranslationFilter *mtf = new MyoTranslationFilter(NULL);

    float tolerance = 0.001;
    
    //Valid inputs -- BASIC
    ASSERT_NEAR(0.0, mtf->calcRingDelta(degToRad(90.0), degToRad(90.0)), tolerance);
    ASSERT_NEAR(degToRad(45.0), mtf->calcRingDelta(degToRad(90.0), degToRad(45.0)), tolerance);

    //BOUNDARY

    // Curr >= Base
    ASSERT_NEAR(degToRad(179.9), mtf->calcRingDelta(degToRad(179.9), degToRad(0.0)), tolerance);
    ASSERT_NEAR(degToRad(179.9), mtf->calcRingDelta(degToRad(189.9), degToRad(10.0)), tolerance);
    ASSERT_NEAR(degToRad(179.9), mtf->calcRingDelta(degToRad(358.9), degToRad(179.0)), tolerance);
 
    ASSERT_NEAR(degToRad(-179.0), mtf->calcRingDelta(degToRad(181.0), degToRad(0.0)), tolerance);
    ASSERT_NEAR(degToRad(-1.0), mtf->calcRingDelta(degToRad(359.0), degToRad(0.0)), tolerance);
    ASSERT_NEAR(degToRad(-179.0), mtf->calcRingDelta(degToRad(359.0), degToRad(178.0)), tolerance);
                    
    // Curr < Base  
    ASSERT_NEAR(degToRad(-179.9), mtf->calcRingDelta(degToRad(0.0), degToRad(179.9)), tolerance);
    ASSERT_NEAR(degToRad(-179.9), mtf->calcRingDelta(degToRad(10.0), degToRad(189.9)), tolerance);
    ASSERT_NEAR(degToRad(-179.9), mtf->calcRingDelta(degToRad(179.0), degToRad(358.9)), tolerance);
 
    ASSERT_NEAR(degToRad(179.0), mtf->calcRingDelta(degToRad(0.0), degToRad(181.0)), tolerance);
    ASSERT_NEAR(degToRad(1.0), mtf->calcRingDelta(degToRad(0.0), degToRad(359.0)), tolerance);
    ASSERT_NEAR(degToRad(179.0), mtf->calcRingDelta(degToRad(178.0), degToRad(359.0)), tolerance);

    delete mtf;
}
