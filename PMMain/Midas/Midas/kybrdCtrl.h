#ifndef _KEYBOARD_CONTROLLER_H
#define _KEYBOARD_CONTROLLER_H

#include <Windows.h>
#include <vector>
#include "MidasCommon.h"

class KybrdCtrl 
{
public:
    KybrdCtrl();

    /*
    Input: 
        KeybdCmd: The command to configure the Keyboard Controller state to.
        releaseKeys: Boolean to state if all keys pressed should be released.
            ex - inputting Copy would either result in (Ctrl down, '+' down, 
            Ctrl up, '+' up) or just (Ctrl down, '+' down)
    Output:
        none.
    Purpose:
        Use this function to configure a Keyboard Controller. Once configured, 
        sedData() can be called to execute the command.
    */
    void setKeyCmd(kybdCmds keybdCmd, bool releaseKeys = true);

    /*
    Input: 
        c: The character to be pressed - any character is acceptable, as it will
        be used as an UNICODE character.
        releaseKeys: Boolean to state if character pressed should be released.
    Output:
        none.
    Purpose:
        Use this function to configure a Keyboard Controller. Once configured, 
        sedData() can be called to execute the command.
    */
    void setKeyChar(char c, bool releaseKeys = true);

    /*
    Input:
        none.
    Output:
        integer represending status of the action, as per kybdStatus.
    Purpose:
        Use this function to execute the configuration of the Keyboard Controller,
        by actually sending the data to Windows.
    */
    int sendData();

    /*
    Input:
        none.
    Output:
        returns the kiVec, in case caller needs it.
    Purpose:
        Allow caller to have access to kiVec. Should be used with caution.
    */
    std::vector<KEYBDINPUT> getKeyInputVec();

    /*
    Input:
        ki_arr: Pre-allocated array, to be populated with INPUT values,
            based on kiVec. This array MUST be at least as large as kiVec,
            or else the function returns an ARRAY_TO_SMALL error.
        len_arr: length of the ki_arr.
    Output:
        returns the kiVec, in case caller needs it.
    Purpose:
        Use this functino to populate a pre-allocated INPUT array. This array
        can be used to utilize Windows API calls to actually send INPUT data.
    */
    int getKeyInputArr(INPUT * ki_arr, int len_arr);

private:
    /*
    Purpose: 
        Input a Control command into kiVec.
    */
    void inputCtrl();

    /*
    Purpose: 
        Input a Left Shift command into kiVec.
    */
    void inputLShift();

    /*
    Purpose: 
        Input an Alt command into kiVec.
    */
    void inputAlt();

    /*
    Purpose: 
        Input a Windows-key command into kiVec.
    */
    void inputWindows();

    /*
    Purpose: 
        Input a virtual key code to kiVec, populating the wVK and wVScan values
        of the KEYBDINPUT properly.
    */
    void inputVK(int vk);

    /*
    Purpose: 
        Go through kiVec, appending the mirror of the current contents, making
        all keys be RELEASED, rather than pressed in the new half.
    */
    void setMirroredUpKeys();

    KEYBDINPUT ki;
    std::vector<KEYBDINPUT> kiVec;
    bool kiWillReleaseKeys;
};


#endif /* _KEYBOARD_CONTROLLER_H */