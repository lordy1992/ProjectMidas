#ifndef _KYBRD_CONTROLLER_H
#define _KYBRD_CONTROLLER_H

#include <Windows.h>
#include <vector>
#include "MidasCommon.h"

/**
 * Handles sending keyboard command data to Windows OS.
 */
class KybrdCtrl 
{
public:
    /* Basic Construction/Destruction */
    KybrdCtrl();
    ~KybrdCtrl();

    /**
     * This function configures the keyboard controller state based on high-level commands, such as
     * Copy with no key release (which results in a keyboard state of Ctrl + 'c' pressed). Once 
     * the keyboard command is set, it can be executed by calling sendData(). If releaseKeys is true,
     * the keyboard command consists of a key press followed by a release of the same key(s). Otherwise,
     * the key(s) is (or are) pressed down and never released.
     *
     * @param keybdCmd The high-level command to the keyboard.
     * @param releaseKeys If this is true, the key(s) will be released after being pressed. Otherwise, 
     * it (or they) will just be pressed.
     * @param holdShift If set to true, shift will be held before all keys in the keybdCmd will be added,
     * then at the end it will be released.
     */
    void setKeyCmd(kybdCmds keybdCmd, bool releaseKeys = true, bool holdShift = false);

    /**
     * This function configures the keyboard controller state based on a single character
     * key. It results in a key press of a single character, c. If releaseKeys is true, the 
     * key press will be followed by a key release. This command sets the keyboard command, but 
     * to execute the command the user must call sendData().
     *
     * @param c The character to be pressed. This is not confined to ASCII, as this
     * character will be treated as a UNICODE character.
     * @param releaseKeys If this is true, the key press will be followed by a release. Otherwise,
     * it will just cause a key press.
     */
    void setKeyChar(unsigned char c, bool releaseKeys = true);

    /**
     * This executes the keyboard commands set for the keyboard controller by sending the
     * data to Windows.
     *
     * @return An integer representing the status of the action, as per kybdStatus.
     */
    int sendData();

private:
    /**
    * Returns the vector of keyboard inputs. Should be used with caution.
    *
    * @return The kiVec, vector of keyboard inputs.
    */
    std::vector<KEYBDINPUT> getKeyInputVec();

    /**
    * Populates a pre-allocated INPUT array. This array can be used to utilize Windows
    * API calls to send the INPUT data.
    *
    * @param kiArr A pre-allocated array to be populated with INPUT values, from the
    * vector of keyboard inputs formed through setting commands in the keyboard controller. This
    * array must be at least as large as the vector of commands or the function will return an
    * ARRAY_TOO_SMALL error.
    * @param lenArr The length of the supplied array.
    */
    int getKeyInputArr(INPUT * kiArr, int lenArr);

    /**
    * Inputs a virtual key code into the vector of keyboard commands.
    */
    void inputVK(unsigned int vk);

    /**
    * Appends the 'mirror' of each key currently in the vector of keyboard commands.
    * The 'mirror' of a key in the keyboard command vector is the same key, released 
    * instead of pressed. This method enforces that every key is released after it is 
    * pressed, in the correct order.
    */
    void setMirroredUpKeys();  

    KEYBDINPUT ki;
    std::vector<KEYBDINPUT> kiVec;
    bool kiWillReleaseKeys;
};


#endif /* _KYBRD_CONTROLLER_H */