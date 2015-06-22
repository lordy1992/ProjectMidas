#ifndef _MIDAS_COMMON_H
#define _MIDAS_COMMON_H

#include <string>
#include <map>

#define MOUSE_INDICATOR_SIZE 130
#define INFO_INDICATOR_WIDTH   150
#define INFO_INDICATOR_HEIGHT  35
#define PROF_INDICATOR_WIDTH   250
#define PROF_INDICATOR_HEIGHT  35
#define WIDGET_BUFFER 5
#define DISTANCE_DISPLAY_WIDTH 250
#define DISTANCE_DISPLAY_HEIGHT 40
#define GUI_HEIGHT_OFFSET_FROM_BOTTOM (MOUSE_INDICATOR_SIZE + INFO_INDICATOR_HEIGHT + 3*WIDGET_BUFFER)

#define SPECIFIC_PROFILE_ICON_SIZE 60
#define PROFILE_ICON0_ACTIVE	"Resources\\icons\\Ring_Post2.png"	
#define PROFILE_ICON0_INACTIVE	"Resources\\icons\\RingBW2.png"		
#define PROFILE_ICON1_ACTIVE	"Resources\\icons\\Pencil_Post.png"
#define PROFILE_ICON1_INACTIVE	"Resources\\icons\\PencilBW.png"

class KeyboardVector;

/**
 * Enumerates the types of commands that can be sent to control
 * peripherals on Windows.
 */
enum commandType {
    KYBRD_CMD,
    KYBRD_GUI_CMD,
    MOUSE_CMD,
    STATE_CHANGE,
	PROFILE_CHANGE,
    NONE,
    UNKNOWN_COMMAND
};

enum profileCmds {
	MOVE_PROFILE_FORWARD,
	MOVE_PROFILE_BACKWARD
};

/**
*  Keyboard GUI commands, used to perform actions across the 
*  Midas main/GUI threads.
*/
enum kybdGUICmds {
    SWAP_RING_FOCUS,
    CHANGE_WHEELS,
    SELECT,
    HOLD_SELECT
};

/**
 * High-level keyboard commands that represent common actions.
 */
enum kybdCmds {
    NO_CMD = 0,
    UNDO = 1,
    REDO = 2,
    ZOOM_IN = 4,
    ZOOM_OUT = 8,
    ZOOM_100 = 16,
    ESCAPE = 32,
    ENTER = 64,
    TAB = 128,
    SWITCH_WIN_FORWARD = 256,
    SWITCH_WIN_REVERSE = 512,
    COPY = 1024,
    PASTE = 2048,
    CUT = 4096,
    FILE_MENU = 8192,
    NEW_BROWSER = 16384,
    GOTO_ADDR_BAR = 32768,
    LOCK_DESKTOP = 65536,
    EDIT_MENU = 131072,
    VIEW_MENU = 262144,
    WIN_HOME = 524288,
    HIDE_APPS = 1048576,
    CONTROL = 2097152,
    VOLUME_UP = 4194304,
    VOLUME_DOWN = 8388608,
    BACKSPACE = 16777216,
    UP_ARROW = 33554432,
    RIGHT_ARROW = 67108864,
    DOWN_ARROW = 134217728,
    LEFT_ARROW = 268435456,
	INPUT_VECTOR = 536870912 // 2^29 ... NEXT = 1073741824
};

/**
 * Mouse commands caused by buttons, wheels and other features
 * that are a part of a mouse.
 */
enum mouseCmds {
    LEFT_CLICK,
    RIGHT_CLICK,
    MIDDLE_CLICK,
    LEFT_HOLD,
    RIGHT_HOLD,
    MIDDLE_HOLD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_HOR,
    MOVE_VERT,
    SCROLL_LEFT,
    SCROLL_RIGHT,
    SCROLL_UP,
    SCROLL_DOWN,
    LEFT_RELEASE,
    RIGHT_RELEASE,
    MIDDLE_RELEASE,
    RELEASE_LRM_BUTS,
	MOVE_ABSOLUTE
};

/**
 * This represents the state/mode of the application. Each mode has
 * different rules that must be enforced by each Midas device.
 *
 * NOTE: IF THIS IS CHANGED: any code that follows a similar format to
 * (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
 * NEEDS to be updated, or else errors can occur. For example, GestureSeqRecorder.cpp uses this type
 * of code to loop through the midasModes.
 */
enum midasMode {
    LOCK_MODE,      /**< The mouse is locked from movement. */
    MOUSE_MODE,     /**< The mouse is free to move around. */
    KEYBOARD_MODE,  /**< The user has opened the virtual keyboard. */
    GESTURE_MODE,    /**< Gesture sequences should result in specific keyboard commands. */
    GESTURE_HOLD_ONE, /**< In Gesture mode, specific gesture has been held */
    GESTURE_HOLD_TWO,
    GESTURE_HOLD_THREE,
    GESTURE_HOLD_FOUR,
    GESTURE_HOLD_FIVE
};

static std::string modeToString(midasMode mm)
{
    switch (mm)
    {
    case LOCK_MODE:   
        return "Locked";
    case MOUSE_MODE:  
        return "Unlocked - Mouse";
    case KEYBOARD_MODE:  
        return "Unlocked - Keyboard";
    case GESTURE_MODE:  
        return "Unlocked";
    case GESTURE_HOLD_ONE:
        return "Hold1 Mode";
    case GESTURE_HOLD_TWO:
        return "Hold2 Mode";
    case GESTURE_HOLD_THREE:
        return "Hold3 Mode";
    case GESTURE_HOLD_FOUR:
        return "Hold4 Mode";
    case GESTURE_HOLD_FIVE:
        return "Hold5 Mode";
    default:
        return "default";
    }
}

/**
 * A status used in the Keyboard Controller, with different
 * error types that can be returned.
 */
enum kybdStatus {
    SUCCESS,                    /**< No error. */
    ARRAY_TOO_SMALL,            /**< A provided buffer is too small for the requested data. */
    N_PRESSES_MATCH_RELEASES    /**< The number of key presses does not match the number of key releases.*/
};

typedef union action{
	kybdCmds kybd;
	kybdGUICmds kybdGUI;
	mouseCmds mouse;
	midasMode mode;
	profileCmds profile;
} action;

/**
 * A simple point consisting of two coordinates, x and y.
 */
struct point {
    int x, y;
    point(int xVal = 0, int yVal = 0) : x(xVal), y(yVal) { }
};

struct vector2D {
	double x, y;
	vector2D(int xVal = 0, int yVal = 0) : x(xVal), y(yVal) { }
};

struct vector3D {
	double x, y, z;
	vector3D(double xVal = 0.0, double yVal = 0.0, double zVal = 0.0) : x(xVal), y(yVal), z(zVal) { }
};

struct keyboardAngle {
    int angle;
    bool ringThreshReached = true;
    
    // below are temporary values for debugging purposes!
    int x, y;
};

#endif /* _MIDAS_COMMON_H */