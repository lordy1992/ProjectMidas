#include "kybrdCtrl.h"
#include <iostream>

kybrdCtrl::kybrdCtrl()
{
    this->kiVec.clear();
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    kiWillReleaseKeys = false;
}


void kybrdCtrl::setKeyCmd(kybdCmds kybdCmd, bool releaseKeys)
{
    this->kiWillReleaseKeys = releaseKeys;
    this->kiVec.clear();
    ZeroMemory(&ki, sizeof(KEYBDINPUT));

    switch (kybdCmd) 
    {
    case UNDO:
        inputCtrl();
        inputVK(0x5A); // 'Z' key
        break;
    case REDO:
        inputCtrl();
        inputVK(0x59); // 'Y' key
        break;
    case ZOOM_IN:
        inputCtrl();
        inputVK(VK_OEM_PLUS);
        break;
    case ZOOM_OUT:
        inputCtrl();
        inputVK(VK_OEM_MINUS);
        break;
    case ZOOM_100:
        inputCtrl();
        inputVK(0x30); // '0' key
        break;
    case ESCAPE:
        inputVK(VK_ESCAPE);
        break;
    case ENTER:
        inputVK(VK_RETURN);
        break;
    case TAB:
        inputVK(VK_TAB);
        break;
    case SWITCH_WIN_FORWARD:
        // THIS ONE IS NOT WORKING TODO
        inputAlt();
        //inputWindows(); // hijack test
        inputVK(VK_TAB);
        break;
    case SWITCH_WIN_REVERSE:
        // THIS ONE IS NOT WORKING TODO
        inputAlt();
        //inputWindows(); // hijack test
        inputLShift();
        inputVK(VK_TAB);
        break;
    case COPY:
        inputCtrl();
        inputVK(0x43); // 'C' key
        this->ki.wScan = MapVirtualKey(0x43, MAPVK_VK_TO_VSC);
        break;
    case PASTE:
        inputCtrl();
        inputVK(0x56); // 'V' key
        break;
    case CUT:
        inputCtrl();
        inputVK(0x58); // 'X' key
        break;
    case FILE_MENU:
        inputAlt();
        inputVK(0x46); // 'F' key
        break;
    case NEW_BROWSER:
        inputCtrl();
        inputVK(0x4E); // 'N' key
        break;
    case GOTO_ADDR_BAR:
        inputAlt();
        inputVK(0x44); // 'D' key
        break;
    case LOCK_DESKTOP:
        // THIS ONE IS NOT WORKING TODO
        inputWindows();
        ZeroMemory(&ki, sizeof(KEYBDINPUT));
        inputVK(0x4C); // 'L' key
        this->ki.wScan = MapVirtualKey(0x4C, MAPVK_VK_TO_VSC);
        break;
    case EDIT_MENU:
        inputAlt();
        inputVK(0x45); // 'E' key
        break;
    case VIEW_MENU:
        inputAlt();
        inputVK(0x56); // 'V' key
        break;
    case WIN_HOME:
        inputWindows();
        break;
    case HIDE_APPS:
        inputWindows();
        inputVK(0x44); // 'D' key
        break;
    default:
        break;
    }

    // Ensure that all keys that were pressed down are now 'pressed up'
    if (releaseKeys)
    {
        setMirroredUpKeys();
    }
}

void kybrdCtrl::setKeyChar(char c, bool releaseKeys)
{
    this->kiWillReleaseKeys = releaseKeys;
    this->kiVec.clear();
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    int cInt = int(c);

    this->ki.wScan = cInt;
    this->ki.dwFlags = KEYEVENTF_UNICODE;
    this->kiVec.push_back(ki);

    if (releaseKeys)
    {
        setMirroredUpKeys();
    }
}

int kybrdCtrl::sendData()
{    
    // Make input array, much larger than any anticipated data will ever actually be.
    int numKeys = this->kiVec.size();
    INPUT* kiArr = (INPUT*)malloc(sizeof(INPUT) * numKeys);

    int status = getKeyInputArr(kiArr, numKeys);
    if (status == 0)
    {
        if (this->kiWillReleaseKeys) 
        {
            if (numKeys % 2 != 0)
            {
                // error - all pushes should be matched with releases if this code is to execute.
                return -2;
            }
            // send push key data first, delay, then send releases. This reduces issues with commands like "alt-tab" and "win-'l'"
            int numSent = SendInput(numKeys/2, kiArr, sizeof(INPUT));
            std::cout << "sent " << numSent << " key-presses of data" << std::endl;
            Sleep(20); // 20 ms should do it
            numSent = SendInput(numKeys/2, kiArr + (numKeys/2), sizeof(INPUT));
            std::cout << "sent " << numSent << " key-presses of data" << std::endl;
        }
        else
        {
            // send all key data at once
            int numSent = SendInput(numKeys, kiArr, sizeof(INPUT));
            std::cout << "sent " << numSent << " key-presses of data" << std::endl;
        }
    }

    free(kiArr);

    return status;
}

void kybrdCtrl::setMirroredUpKeys()
{
    int numKeysToMirror = this->kiVec.size();
    for (int i = numKeysToMirror - 1; i >= 0; i--) 
    {
        this->ki = this->kiVec.at(i);
        this->ki.dwFlags |= KEYEVENTF_KEYUP;
        this->kiVec.push_back(ki);
    }
}

void kybrdCtrl::inputCtrl()
{
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    this->ki.wVk = VK_CONTROL;
    this->ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);

    this->kiVec.push_back(ki);
}

void kybrdCtrl::inputLShift()
{
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    this->ki.wVk = VK_LSHIFT;
    this->ki.wScan = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);

    this->kiVec.push_back(ki);
}

void kybrdCtrl::inputAlt()
{
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    this->ki.wVk = VK_MENU;
    this->ki.wScan = MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC);

    this->kiVec.push_back(ki);
}

void kybrdCtrl::inputWindows()
{
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    this->ki.wVk = VK_LWIN;
    this->ki.wScan = MapVirtualKey(VK_LWIN, MAPVK_VK_TO_VSC);

    this->kiVec.push_back(ki);
}

void kybrdCtrl::inputVK(int vk)
{
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    this->ki.wVk = vk;
    this->ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);

    this->kiVec.push_back(ki);
}

std::vector<KEYBDINPUT> kybrdCtrl::getKeyInputVec()
{
    return this->kiVec;
}

int kybrdCtrl::getKeyInputArr(INPUT * ki_arr, int len_arr)
{
    if (len_arr < this->kiVec.size()) return -1;

    for (int i = 0; i < this->kiVec.size(); i++)
    {
        ki_arr[i].type = INPUT_KEYBOARD;
        ki_arr[i].ki = this->kiVec.at(i);
    }

    return 0;
}
