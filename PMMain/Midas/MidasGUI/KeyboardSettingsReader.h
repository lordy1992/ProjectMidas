#pragma once
#include <vector>
#include "ringData.h"
#include <fstream>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>

class keyboardSettingsReader
{
    enum currentRing
    {
        ringIn,
        ringOut
    };

public:
    keyboardSettingsReader();
    ~keyboardSettingsReader();
    void readKeyboardSetupFile(std::vector<ringData> ringDataHandle);
};

