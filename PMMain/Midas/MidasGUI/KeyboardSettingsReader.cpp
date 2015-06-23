#include "KeyboardSettingsReader.h"
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;

KeyboardSettingsReader::KeyboardSettingsReader()
{
}


KeyboardSettingsReader::~KeyboardSettingsReader()
{
}


void KeyboardSettingsReader::readKeyboardSetupFile(std::vector<ringData>& ringDataHandle , std::string fileName)
{

    std::ifstream ringDataFile(fileName);
    std::vector<std::string> temp;
    std::string line;

    currentRing currentRing = ringOut;
    bool holdkeyCheck = false;

    ringData::keyboardValue keyTemp('\0');
    ringData *ringTemp = new ringData();

    if (ringDataFile.is_open())
    {
        while (!ringDataFile.eof())
        {
            getline(ringDataFile, line);
            boost::split(temp, line, std::bind2nd(std::equal_to<char>(), ' '));

            for (int i = 0; i < temp.size(); i++)
            {
                for (char & key : temp[i])
                {

                    if (temp[i].size() == 1) //size can only be 1(main) or 2(main & hold)
                    {
                        keyTemp.main = key;

                        if (currentRing == ringIn)
                        {
                            ringTemp->getRingInVectorHandle()->push_back(keyTemp);
                        }
                        else
                        {
                            ringTemp->getRingOutVectorHandle()->push_back(keyTemp);
                        }
                    }
                    else
                    {
                        if (holdkeyCheck == true)
                        {
                            keyTemp.hold = key;

                            if (currentRing == ringIn)
                            {
                                ringTemp->getRingInVectorHandle()->push_back(keyTemp);
                            }
                            else
                            {
                                ringTemp->getRingOutVectorHandle()->push_back(keyTemp);
                            }

                            holdkeyCheck = false;
                        }
                        else
                        {
                            keyTemp.main = key;
                            holdkeyCheck = true;
                        }
                    }
                }
            }

            if (currentRing == ringOut)
            {
                currentRing = ringIn;
            }
            else if (currentRing == ringIn)
            {
                currentRing = ringOut;
                ringDataHandle.push_back(*ringTemp);
                ringTemp = new ringData();
            }
        }
    }
    else
    {
        std::cout << "Error in opening file!";
    }
    ringDataFile.close();
    ringTemp->~ringData();
}