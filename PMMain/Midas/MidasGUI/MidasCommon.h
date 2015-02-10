#ifndef _MIDAS_COMMON_H
#define _MIDAS_COMMON_H

#include <string>

#define MOUSE_INDICATOR_SIZE 100
#define INFO_INDICATOR_WIDTH   250
#define INFO_INDICATOR_HEIGHT  50
#define WIDGET_BUFFER 5
#define GUI_HEIGHT_OFFSET_FROM_BOTTOM (MOUSE_INDICATOR_SIZE + INFO_INDICATOR_HEIGHT + 3*WIDGET_BUFFER)

/**
 * Enumerates the types of commands that can be sent to control
 * peripherals on Windows.
 */
enum commandType {
    KYBRD_CMD,
    KYBRD_GUI_CMD,
    MOUSE_CMD,
    STATE_CHANGE,
    NONE,
    UNKNOWN_COMMAND
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
    UNDO,
    REDO,
    ZOOM_IN,
    ZOOM_OUT,
    ZOOM_100,
    ESCAPE,
    ENTER,
    TAB,
    SWITCH_WIN_FORWARD,
    SWITCH_WIN_REVERSE,
    COPY,
    PASTE,
    CUT,
    FILE_MENU,
    NEW_BROWSER,
    GOTO_ADDR_BAR,
    LOCK_DESKTOP,
    EDIT_MENU,
    VIEW_MENU,
    WIN_HOME,
    HIDE_APPS,
    CONTROL,
    VOLUME_UP,
    VOLUME_DOWN,
    BACKSPACE
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
    RELEASE_LRM_BUTS
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
        return "Mouse Mode";
    case KEYBOARD_MODE:  
        return "Keyboard Mode";
    case GESTURE_MODE:  
        return "Gesture Mode";
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

/**
 * This struct bundles the Midas command data. It represents
 * either all Midas command types.
 */
struct commandData {
    commandType type = commandType::NONE;
    union action{
        kybdCmds kybd;
        kybdGUICmds kybdGUI;
        mouseCmds mouse;
        midasMode mode;
    };
    action action;

    std::string name = "";
};

/**
 * A simple point consisting of two coordinates, x and y.
 */
struct point {
    int x, y;
    point(int xVal = 0, int yVal = 0) : x(xVal), y(yVal) { }
};

struct keyboardAngle {
    int angle;
    bool ringThreshReached = true;
    
    // below are temporary values for debugging purposes!
    int x, y;
};

#endif /* _MIDAS_COMMON_H */