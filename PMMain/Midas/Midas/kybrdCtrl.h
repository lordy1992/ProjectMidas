#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H

#include <Windows.h>
#include <vector>
#include "MidasCommon.h"

class kybrdCtrl 
{
public:
    kybrdCtrl();

    void setKeyCmd(kybdCmds keybdCmd, bool releaseKeys = true);

    void setKeyChar(char c, bool releaseKeys = true);

    int sendData();

    std::vector<KEYBDINPUT> getKeyInputVec();

    int getKeyInputArr(INPUT * ki_arr, int len_arr);

private:
    void inputCtrl();

    void inputLShift();

    void inputAlt();

    void inputWindows();

    void inputVK(int vk);

    void setMirroredUpKeys();

    KEYBDINPUT ki;
    std::vector<KEYBDINPUT> kiVec;
};


#endif