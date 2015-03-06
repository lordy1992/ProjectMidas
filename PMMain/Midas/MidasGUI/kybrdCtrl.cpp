#include "kybrdCtrl.h"
#include <iostream>
#include <string>
#include <fstream>
KybrdCtrl::KybrdCtrl()
{
    this->kiVec.clear();
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    kiWillReleaseKeys = false;
}

KybrdCtrl::~KybrdCtrl()
{
}

void KybrdCtrl::setKeyCmd(kybdCmds kybdCmd, bool releaseKeys)
{
    this->kiWillReleaseKeys = releaseKeys;
    this->kiVec.clear();
    ZeroMemory(&ki, sizeof(KEYBDINPUT));

    switch (kybdCmd) 
    {
    case UNDO:
        inputVK(VK_CONTROL);
        inputVK(0x5A); // 'Z' key
        break;
    case REDO:
        inputVK(VK_CONTROL);
        inputVK(0x59); // 'Y' key
        break;
    case ZOOM_IN:
        inputVK(VK_CONTROL);
        inputVK(VK_OEM_PLUS);
        break;
    case ZOOM_OUT:
        inputVK(VK_CONTROL);
        inputVK(VK_OEM_MINUS);
        break;
    case ZOOM_100:
        inputVK(VK_CONTROL);
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
        // This only works if uiAccess is set to True.
        inputVK(VK_MENU);
        inputVK(VK_TAB);
        break;
    case SWITCH_WIN_REVERSE:
        // This only works if uiAccess is set to True.
        inputVK(VK_MENU);
        inputVK(VK_LSHIFT);
        inputVK(VK_TAB);
        break;
    case COPY:
        inputVK(VK_CONTROL);
        inputVK(0x43); // 'C' key
        break;
    case PASTE:
        inputVK(VK_CONTROL);
        inputVK(0x56); // 'V' key
        break;
    case CUT:
        inputVK(VK_CONTROL);
        inputVK(0x58); // 'X' key
        break;
    case FILE_MENU:
        inputVK(VK_MENU);
        inputVK(0x46); // 'F' key
        break;
    case NEW_BROWSER:
        inputVK(VK_CONTROL);
        inputVK(0x4E); // 'N' key
        break;
    case GOTO_ADDR_BAR:
        inputVK(VK_MENU);
        inputVK(0x44); // 'D' key
        break;
    case LOCK_DESKTOP:
        // THIS ONE IS NOT WORKING TODO
        inputVK(VK_LWIN);
        inputVK(0x4C); // 'L' key
        break;
    case EDIT_MENU:
        inputVK(VK_MENU);
        inputVK(0x45); // 'E' key
        break;
    case VIEW_MENU:
        inputVK(VK_MENU);
        inputVK(0x56); // 'V' key
        break;
    case WIN_HOME:
        inputVK(VK_LWIN);
        break;
    case HIDE_APPS:
        inputVK(VK_LWIN);
        inputVK(0x44); // 'D' key
        break;
    case CONTROL:
        inputVK(VK_CONTROL);
        break;
    case VOLUME_UP:
        inputVK(VK_VOLUME_UP);
        break;
    case VOLUME_DOWN:
        inputVK(VK_VOLUME_DOWN);
        break;
    case BACKSPACE:
        inputVK(VK_BACK);
        break;
    case UP_ARROW:
        inputVK(VK_UP);
        break;
    case RIGHT_ARROW:
        inputVK(VK_RIGHT);
        break;
    case DOWN_ARROW:
        inputVK(VK_DOWN);
        break;
    case LEFT_ARROW:
        inputVK(VK_LEFT);
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

void KybrdCtrl::setKeyChar(unsigned char c, bool releaseKeys)
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

int KybrdCtrl::sendData()
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
                return kybdStatus::N_PRESSES_MATCH_RELEASES;
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

void KybrdCtrl::setMirroredUpKeys()
{
    int numKeysToMirror = this->kiVec.size();
    for (int i = numKeysToMirror - 1; i >= 0; i--) 
    {
        this->ki = this->kiVec.at(i);
        this->ki.dwFlags |= KEYEVENTF_KEYUP;
        this->kiVec.push_back(ki);
    }
}

void KybrdCtrl::inputVK(unsigned int vk)
{
    ZeroMemory(&ki, sizeof(KEYBDINPUT));
    this->ki.wVk = vk;
    this->ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);

    this->kiVec.push_back(ki);
}

std::vector<KEYBDINPUT> KybrdCtrl::getKeyInputVec()
{
    return this->kiVec;
}

int KybrdCtrl::getKeyInputArr(INPUT * kiArr, int lenArr)
{
    if (lenArr < this->kiVec.size()) return ARRAY_TOO_SMALL;

    for (unsigned int i = 0; i < this->kiVec.size(); i++)
    {
        kiArr[i].type = INPUT_KEYBOARD;
        kiArr[i].ki = this->kiVec.at(i);
    }

    return kybdStatus::SUCCESS;
}



