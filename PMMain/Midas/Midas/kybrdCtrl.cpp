#include "kybrdCtrl.h"

kybrdCtrl::kybrdCtrl()
{
	this->kiVec.clear();
	ZeroMemory(&ki, sizeof(KEYBDINPUT));
}


void kybrdCtrl::setKeyCmd(kybdCmds kybdCmd, bool releaseKeys)
{
	this->kiVec.clear();
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	switch (kybdCmd) {
	case UNDO:
		inputCtrl();
		ZeroMemory(&ki, sizeof(KEYBDINPUT));
		this->ki.wVk = 0x5A; // 'Z' key
		break;
	case REDO:
		inputCtrl();
		ZeroMemory(&ki, sizeof(KEYBDINPUT));
		this->ki.wVk = 0x59; // 'Y' key
		break;
	case ZOOM_IN:
		inputCtrl();
		ZeroMemory(&ki, sizeof(KEYBDINPUT));
		this->ki.wVk = VK_OEM_PLUS;
		break;
	case ZOOM_OUT:
		inputCtrl();
		ZeroMemory(&ki, sizeof(KEYBDINPUT));
		this->ki.wVk = VK_OEM_MINUS;
		break;
	case ZOOM_100:
		break;
	case ESCAPE:
		break;
	case ENTER:
		break;
	case TAB:
		break;
	case SWITCH_WIN_FORWARD:
		break;
	case SWITCH_WIN_REVERSE:
		break;
	case COPY:
		break;
	case PASTE:
		break;
	case CUT:
		break;
	case FILE_MENU:
		break;
	case NEW_BROWSER:
		break;
	case GOTO_ADDR_BAR:
		break;
	case LOCK_DESKTOP:
		break;
	case EDIT_MENU:
		break;
	case VIEW_MENU:
		break;
	}

	this->kiVec.push_back(ki);

	// Ensure that all keys that were pressed down are now 'pressed up'
	if (releaseKeys)
		setMirroredUpKeys();
}

void kybrdCtrl::setKeyChar(char c, bool releaseKeys)
{
	this->kiVec.clear();
	ZeroMemory(&ki, sizeof(KEYBDINPUT));
	int cInt = int(c);

	this->ki.wScan = cInt;
	this->ki.dwFlags = KEYEVENTF_UNICODE;
	this->kiVec.push_back(ki);

	if (releaseKeys)
		setMirroredUpKeys();
}

void kybrdCtrl::setMirroredUpKeys()
{
	int numKeysToMirror = this->kiVec.size();
	for (int i = numKeysToMirror - 1; i >= 0; i--) {
		this->ki = this->kiVec.at(i);
		this->ki.dwFlags |= KEYEVENTF_KEYUP;
		this->kiVec.push_back(ki);
	}
}

void kybrdCtrl::inputCtrl()
{
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	this->ki.wVk = VK_CONTROL;

	this->kiVec.push_back(ki);
}

std::vector<KEYBDINPUT> kybrdCtrl::getKeyInputVec()
{
	return this->kiVec;
}

void kybrdCtrl::getKeyInputArr(INPUT * ki_arr, int len_arr)
{
	if (len_arr < this->kiVec.size()) {
		//TODO - exception?
		return;
	}

	for (int i = 0; i < this->kiVec.size(); i++)
	{
		ki_arr[i].type = INPUT_KEYBOARD;
		ki_arr[i].ki = this->kiVec.at(i);
	}

	return;
}
