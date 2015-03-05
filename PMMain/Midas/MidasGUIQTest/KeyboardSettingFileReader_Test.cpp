#include "KeyboardSettingsReader.h"
#include "RingData.h"
#include <gtest\gtest.h>
#include <iostream>
#include <fstream>

TEST(KeyboardFileReaderTest1, constructor) {
    
    std::ofstream outputFile("testFile.txt");
    
    if (!outputFile)
    {
        //error in creating the file
        perror("Error creating file");
    }

    outputFile << "aA bB cC dD eE fF gG hH iI jJ kK lL mM" << std::endl;
    outputFile << "nN oO pP qQ rR sS tT uU vV wW xX yY zZ" << std::endl;
    outputFile << "`~ 1! 2@ 3# 4$ 5% 6^ 7& 8* 9( 0) -_ =+" << std::endl;
    outputFile << "[{ ]} ;: ' \ | , < .> | \ / ?" << std::endl;
    outputFile.close();

    std::vector<ringData>  test;
    KeyboardSettingsReader reader;
    reader.readKeyboardSetupFile(test, "testFile.txt");
    

    if (remove("testFile.txt") != 0)
    {
        perror("Error deleting file");
    }
    else
    {
        puts("File successfully deleted");
    }
  

    ASSERT_EQ(test.size(), 2); //checks for # of rings

    ASSERT_EQ('a', test[0].getRingOutVectorHandle()->at(0).main); //checks for the main in ringOut for ring 1
    ASSERT_EQ('A',test[0].getRingOutVectorHandle()->at(0).hold); //checks for the hold in ringOut for ring 1
    ASSERT_EQ('n',test[0].getRingInVectorHandle()->at(0).main); //checks for the main in ringOut for ring 1
    ASSERT_EQ('N',test[0].getRingInVectorHandle()->at(0).hold); //checks for the hold in ringOut for ring 1
    ASSERT_EQ('`', test[1].getRingOutVectorHandle()->at(0).main); //checks for the main in ringOut for ring 2
    ASSERT_EQ('~', test[1].getRingOutVectorHandle()->at(0).hold); //checks for the hold in ringOut for ring 2
    ASSERT_EQ('[', test[1].getRingInVectorHandle()->at(0).main); //checks for the main in ringOut for ring 2
    ASSERT_EQ('{', test[1].getRingInVectorHandle()->at(0).hold); //checks for the hold in ringOut for ring 2
}

