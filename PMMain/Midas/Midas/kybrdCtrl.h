#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H

#include <Windows.h>
#include <vector>
#include "MidasCommon.h"

class kybrdCtrl {
public:
    kybrdCtrl();

    void setKeyCmd(kybdCmds keybdCmd, bool releaseKeys = true);

    void setKeyChar(char c, bool releaseKeys = true);

    std::vector<KEYBDINPUT> getKeyInputVec();

    void getKeyInputArr(INPUT * ki_arr, int len_arr);

private:
    void inputCtrl();

    void setMirroredUpKeys();

    KEYBDINPUT ki;
    std::vector<KEYBDINPUT> kiVec;
};


#endif