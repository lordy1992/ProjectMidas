#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "MyoCommon.h"
#include "MidasCommon.h"
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct command {
    std::string type;
    std::vector<std::string> actions;
};

struct gesture {
    std::string type;
    std::string name;
};

struct profileSequence {
    std::string state;
    std::string name;
    std::vector<gesture> gestures;
    command cmd;
};

struct profile {
    std::string profileName;
    std::vector<profileSequence> profileSequences;
};

std::map<std::string, commandType> profileCommandToCommandTypeMap =
{
    { "keyboard", KYBRD_CMD },
    { "keyboardGui", KYBRD_GUI_CMD },
    { "mouse", MOUSE_CMD },
    { "stateChange", STATE_CHANGE },
    { "none", NONE }
};

std::map<std::string, kybdGUICmds> profileActionToKybdGui =
{
    { "swapRingFocus", SWAP_RING_FOCUS },
    { "changeWheels", CHANGE_WHEELS },
    { "select", SELECT },
    { "holdSelect", HOLD_SELECT },
};

std::map<std::string, kybdCmds> profileActionToKybd =
{
    { "none", NO_CMD },
    { "undo", UNDO },
    { "redo", REDO },
    { "zoomIn", ZOOM_IN },
    { "zoomOut", ZOOM_OUT },
    { "zoom100", ZOOM_100 },
    { "escape", ESCAPE },
    { "enter", ENTER },
    { "tab", TAB },
    { "switchWinForward", SWITCH_WIN_FORWARD },
    { "switchWinReverse", SWITCH_WIN_REVERSE },
    { "copy", COPY },
    { "paste", PASTE },
    { "cut", CUT },
    { "fileMenu", FILE_MENU },
    { "newBrowser", NEW_BROWSER },
    { "gotoAddrBar", GOTO_ADDR_BAR },
    { "lockDesktop", LOCK_DESKTOP },
    { "editMenu", EDIT_MENU },
    { "viewMenu", VIEW_MENU },
    { "winHome", WIN_HOME },
    { "hideApps", HIDE_APPS },
    { "control", CONTROL },
    { "volumeUp", VOLUME_UP },
    { "volumeDown", VOLUME_DOWN },
    { "backspace", BACKSPACE },
    { "upArrow", UP_ARROW },
    { "downArrow", DOWN_ARROW },
    { "rightArrow", RIGHT_ARROW },
    { "leftArrow", LEFT_ARROW }
};

std::map<std::string, mouseCmds> profileActionToMouseCommands =
{
    { "leftClick", LEFT_CLICK },
    { "rightClick", RIGHT_CLICK },
    { "middleClick", MIDDLE_CLICK },
    { "leftHold", LEFT_HOLD },
    { "rightHold", RIGHT_HOLD },
    { "middleHold", MIDDLE_HOLD },
    { "moveLeft", MOVE_LEFT },
    { "moveRight", MOVE_RIGHT },
    { "moveUp", MOVE_UP },
    { "moveDown", MOVE_DOWN },
    { "moveHor", MOVE_HOR },
    { "moveVert", MOVE_VERT },
    { "scrollLeft", SCROLL_LEFT },
    { "scrollRight", SCROLL_RIGHT },
    { "scrollUp", SCROLL_UP },
    { "scrollDown", SCROLL_DOWN },
    { "leftRelease", LEFT_RELEASE },
    { "rightRelease", RIGHT_RELEASE },
    { "middleRelease", MIDDLE_RELEASE },
    { "releaseLrmButs", RELEASE_LRM_BUTS }
};

std::map<std::string, midasMode> profileActionToStateChange =
{
    { "lockMode", LOCK_MODE },
    { "mouseMode", MOUSE_MODE },
    { "keyboardMode", KEYBOARD_MODE },
    { "gestureMode", GESTURE_MODE },
    { "gestureHoldOne", GESTURE_HOLD_ONE },
    { "gestureHoldTwo", GESTURE_HOLD_TWO },
    { "gestureHoldThree", GESTURE_HOLD_THREE },
    { "gestureHoldFour", GESTURE_HOLD_FOUR },
    { "gestureHoldFive", GESTURE_HOLD_FIVE }
};

std::map <std::string, SeqElement::PoseLength> profileGestureTypeToPoseLength = 
{
    { "tap", SeqElement::PoseLength::TAP },
    { "hold", SeqElement::PoseLength::HOLD },
    { "immediate", SeqElement::PoseLength::IMMEDIATE }
};

std::map <std::string, Pose::Type> profileGestureNameToType =
{
    { "fingersSpread", Pose::Type::fingersSpread },
    { "fist", Pose::Type::fist },
    { "rest", Pose::Type::rest },
    { "thumbToPinky", Pose::Type::thumbToPinky },
    { "waveIn", Pose::Type::waveIn },
    { "waveOut", Pose::Type::waveOut }
};

class ProfileManager {

public:
    ProfileManager();
    ~ProfileManager();

    void loadProfilesFromFile(std::string fileName);

    std::vector<profile>* getProfiles();

private:
    profile extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName);

    profileSequence extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
        std::string sequenceState, std::string sequenceName);

    std::vector<profile> profiles;
};

#endif /* PROFILE_MANAGER_H */