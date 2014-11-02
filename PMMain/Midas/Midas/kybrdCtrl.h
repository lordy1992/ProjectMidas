#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H

#include <Windows.h>
#include <vector>

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
	VIEW_MENU
};

class kybrdCtrl {
public:
	kybrdCtrl();

	void setKeyCmd(kybdCmds keybdCmd, bool releaseKeys = true);

	void setKeyChar(char c, bool releaseKeys = true);

	int sendData();

	std::vector<KEYBDINPUT> getKeyInputVec();

	int getKeyInputArr(INPUT * ki_arr, int len_arr);

private:
	void inputCtrl();

	void setMirroredUpKeys();

	KEYBDINPUT ki;
	std::vector<KEYBDINPUT> kiVec;
};


#endif