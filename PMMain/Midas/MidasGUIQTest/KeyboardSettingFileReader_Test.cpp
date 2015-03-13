#include "KeyboardSettingsReader.h"
#include "RingData.h"
#include <gtest\gtest.h>

TEST(KeyboardFileReaderTest, constructor) {
    
    std::vector<ringData>  test;
    KeyboardSettingsReader reader;
    reader.readKeyboardSetupFile(test);

    ASSERT_EQ(test.size(),2);
}