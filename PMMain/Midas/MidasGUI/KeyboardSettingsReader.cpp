#include "KeyboardSettingsReader.h"


keyboardSettingsReader::keyboardSettingsReader()
{
}


keyboardSettingsReader::~keyboardSettingsReader()
{
}
void keyboardSettingsReader::readKeyboardSetupFile(std::vector<ringData> ringDataHandle)
{

    std::ifstream readRingData("C:/Users/Owen/Documents/keyboardData.txt");
    std::vector<std::string> temp;
    std::string line;

    currentRing currentRing = ringIn;
    bool holdkey_check = false;

    ringData::keyboard_value key_temp;

    ringDataHandle.push_back(ringData());

    int ringCount = 0;


    if (readRingData.is_open())
    {
        while (!readRingData.eof())
        {
            getline(readRingData, line);
            boost::split(temp, line, std::bind2nd(std::equal_to<char>(), ' '));

            for (int i = 0; i < temp.size(); i++)
            {
                for (char & key : temp[i])
                {
                    if (temp[i].size() == 1)
                    {
                        key_temp.main = key;

                        if (currentRing == ringIn)
                        {
                            ringDataHandle[ringCount].getRingInVectorHandle()->push_back(key_temp);
                        }
                        else
                        {
                            ringDataHandle[ringCount].getRingOutVectorHandle()->push_back(key_temp);
                        }
                    }
                    else
                    {
                        if (holdkey_check == true)
                        {
                            key_temp.hold = key;

                            if (currentRing == ringIn)
                            {
                                ringDataHandle[ringCount].getRingInVectorHandle()->push_back(key_temp);
                            }
                            else
                            {
                                ringDataHandle[ringCount].getRingOutVectorHandle()->push_back(key_temp);
                            }

                            holdkey_check = false;
                        }
                        else
                        {
                            key_temp.main = key;
                            holdkey_check = true;
                        }

                    }
                }
              
            }

            if (currentRing == ringIn)
            {
                currentRing = ringOut;
            }
            else if (currentRing == ringOut)
            {
                currentRing = ringIn;
                ringCount++;
                ringDataHandle.push_back(ringData());
            }
        }
    }
    else
    {
        std::cout << "Error in opening file!";
    }

    readRingData.close();

}