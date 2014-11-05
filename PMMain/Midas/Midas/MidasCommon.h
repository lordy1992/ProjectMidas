#ifndef _MIDAS_COMMON_H
#define _MIDAS_COMMON_H

enum commandType {
    KEYBOARD_COMMAND,
    MOUSE_COMMAND
};

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
    HIDE_APPS
};

enum mouseCmds {
    LEFT_CLICK,
    RIGHT_CLICK,
    MIDDLE_CLICK,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN
};

enum midasMode {
    LOCK_MODE,
    MOUSE_MODE,
    KEYBOARD_MODE,
    GESTURE_MODE
};

struct commandData {
    commandType type;
    union {
        kybdCmds kbd;
        mouseCmds mouse;
    };
};

struct point {
    int x, y;
    point(int xVal = 0, int yVal = 0) : x(xVal), y(yVal) { }
};

#endif /* _MIDAS_COMMON_H */