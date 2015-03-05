#pragma once
#include <vector>
#include "ringData.h"
#include <fstream>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>

class KeyboardSettingsReader
{
    enum currentRing
    {
        ringIn,
        ringOut
    };

public:
    KeyboardSettingsReader();
    ~KeyboardSettingsReader();
    void readKeyboardSetupFile(std::vector<ringData> &ringDataHandle, std::string fileName );
};

