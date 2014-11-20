#ifndef _MIDAS_COMMON_H
#define _MIDAS_COMMON_H

/**
 * Enumerates the types of commands that can be sent to control
 * peripherals on Windows.
 */
enum commandType {
    KEYBOARD_COMMAND,
    MOUSE_COMMAND,
    UNKNOWN_COMMAND
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
    CONTROL
};

/**
 * Mouse commands caused by buttons, wheels and other features
 * that are a part of a mouse.
 */
enum mouseCmds {
    LEFT_CLICK,
    RIGHT_CLICK,
    MIDDLE_CLICK,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    SCROLL_LEFT,
    SCROLL_RIGHT,
    SCROLL_UP,
    SCROLL_DOWN,
    LEFT_RELEASE,
    RIGHT_RELEASE,
    MIDDLE_RELEASE
};

/**
 * This represents the state/mode of the application. Each mode has
 * different rules that must be enforced by each Midas device.
 */
enum midasMode {
    LOCK_MODE,      /**< The mouse is locked from movement. */
    MOUSE_MODE,     /**< The mouse is free to move around. */
    KEYBOARD_MODE,  /**< The user has opened the virtual keyboard. */
    GESTURE_MODE    /**< Gesture sequences should result in specific keyboard commands. */
};

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
 * This struct bundles the command data that can be sent to Windows. It represents
 * either a mouse or keyboard command.
 */
struct commandData {
    commandType type;
    union {
        kybdCmds kbd;
        mouseCmds mouse;
    };
};

/**
 * A simple point consisting of two coordinates, x and y.
 */
struct point {
    int x, y;
    point(int xVal = 0, int yVal = 0) : x(xVal), y(yVal) { }
};

#endif /* _MIDAS_COMMON_H */