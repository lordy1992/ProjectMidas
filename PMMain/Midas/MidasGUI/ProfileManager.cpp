#include "ProfileManager.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <fstream>

std::map<std::string, commandType> profileCommandToCommandTypeMap =
{
    { "keyboard", KYBRD_CMD },
    { "keyboardGui", KYBRD_GUI_CMD },
    { "mouse", MOUSE_CMD },
    { "stateChange", STATE_CHANGE },
	{ "profileChange", PROFILE_CHANGE },
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
    { "leftArrow", LEFT_ARROW },
	{ "inputVector", INPUT_VECTOR }
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

std::map<std::string, profileCmds> profileActionToProfileChange =
{
	{ "moveProfileForward", MOVE_PROFILE_FORWARD },
	{ "moveProfileBackward", MOVE_PROFILE_BACKWARD }
};

std::map <std::string, PoseLength> profileGestureTypeToPoseLength =
{
    { "tap", PoseLength::TAP },
    { "hold", PoseLength::HOLD },
    { "immediate", PoseLength::IMMEDIATE }
};

std::map <std::string, Pose::Type> profileGestureNameToType =
{
    { "fingersSpread", Pose::Type::fingersSpread },
    { "fist", Pose::Type::fist },
    { "rest", Pose::Type::rest },
    { "doubleTap", Pose::Type::doubleTap },
    { "waveIn", Pose::Type::waveIn },
    { "waveOut", Pose::Type::waveOut }
};

ProfileManager::ProfileManager() { }

ProfileManager::~ProfileManager() { }

void ProfileManager::loadProfilesFromFile(std::string fileName)
{
    using boost::property_tree::ptree;

    profiles.clear();

    std::ifstream profileFile(fileName.c_str());

    ptree tr;
    read_xml(profileFile, tr);

    BOOST_FOREACH(const ptree::value_type & vt, tr.get_child("profiles")) {
        if (vt.first == "profile")
        {
            std::string profileName = vt.second.get<std::string>("<xmlattr>.name");
            profiles.push_back(extractProfileInformation(vt, profileName));
        }
    }

    profileFile.close();
}

std::vector<profile>* ProfileManager::getProfiles()
{
    return &profiles;
}

profile ProfileManager::extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName)
{
    using boost::property_tree::ptree;

    profile pr; 
    pr.profileName = profileName;

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("sequences")) 
    {
        if (vt.first == "sequence")
        {
            std::string sequenceStateBegin = vt.second.get<std::string>("<xmlattr>.state");
            std::string sequenceName = vt.second.get<std::string>("<xmlattr>.name");
            pr.profileSequences.push_back(extractSequenceInformation(vt, sequenceStateBegin, sequenceName));
        }
    }

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("holds"))
    {
        if (vt.first == "hold")
        {
            hold currHold;
            std::string gesture = vt.second.get<std::string>("<xmlattr>.gesture");
            currHold.gesture = gesture;

            BOOST_FOREACH(const ptree::value_type & angleVt, vt.second)
            {
                if (angleVt.first == "angle")
                {
                    angleAction currAngle;
                    std::string angleType = angleVt.second.get<std::string>("<xmlattr>.type");
                    std::string anglePositive = angleVt.second.get_child("anglePositive").get_value<std::string>();
                    std::string angleNegative = angleVt.second.get_child("angleNegative").get_value<std::string>();
                    currAngle.anglePositive = anglePositive;
                    currAngle.angleNegative = angleNegative;
                    currAngle.type = angleType;
                    currHold.angles.push_back(currAngle);
                }
            }

            pr.holds.push_back(currHold);
        }
    }

    return pr;
}

profileSequence ProfileManager::extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
    std::string sequenceState, std::string sequenceName)
{
    using boost::property_tree::ptree;

    profileSequence seq;
    std::vector<gesture> gestures;

    BOOST_FOREACH(const ptree::value_type & vt, parentSequence.second.get_child("gestures")) {
        if (vt.first == "gesture")
        {
            gesture gest;
            gest.type = vt.second.get<std::string>("<xmlattr>.type");
            gest.name = vt.second.get_value<std::string>();
            
            gestures.push_back(gest);
        }
    }

	std::vector<command> cmds;
	BOOST_FOREACH(const ptree::value_type & pt, parentSequence.second.get_child("commands")) {
		command cmd;
		if (pt.first == "command")
		{
			cmd.type = pt.second.get<std::string>("<xmlattr>.type");
		}

		BOOST_FOREACH(const ptree::value_type & vt, pt.second.get_child("actions")) {
			if (vt.first == "action")
			{
				cmd.actions.push_back(vt.second.get_value<std::string>());
			}
		}
		cmds.push_back(cmd);
	}

    seq.cmds = cmds;
    seq.gestures = gestures;
    seq.state = sequenceState;
    seq.name = sequenceName;

    return seq;
}