#include "KeyboardVector.h"

void KeyboardVector::clear()
{
	this->kiVector.clear();
}

void KeyboardVector::inputCharDown(char c)
{
	KEYBDINPUT ki;
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	int cInt = int(c);

	ki.wScan = cInt;
	ki.dwFlags = KEYEVENTF_UNICODE;
	this->kiVector.push_back(ki);
}

void KeyboardVector::inputCharUp(char c)
{
	KEYBDINPUT ki;
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	int cInt = int(c);

	ki.wScan = cInt;
	ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	this->kiVector.push_back(ki);
}

void KeyboardVector::inputCharDownUp(char c)
{
	// try to insert as VK for down/up as it interacts with some 
	// systems better.
	if (!tryInsertCharAsVK(c))
	{
		KEYBDINPUT ki;
		ZeroMemory(&ki, sizeof(KEYBDINPUT));

		int cInt = int(c);

		ki.wScan = cInt;
		ki.dwFlags = KEYEVENTF_UNICODE;
		this->kiVector.push_back(ki);

		ki.dwFlags |= KEYEVENTF_KEYUP;
		this->kiVector.push_back(ki);
	}
}

void KeyboardVector::inputVKDown(unsigned int vk)
{
	KEYBDINPUT ki;
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	ki.wVk = vk;
	ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
	this->kiVector.push_back(ki);
}

void KeyboardVector::inputVKUp(unsigned int vk)
{
	KEYBDINPUT ki;
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	ki.wVk = vk;
	ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
	ki.dwFlags |= KEYEVENTF_KEYUP;
	this->kiVector.push_back(ki);
}

void KeyboardVector::inputVKDownUp(unsigned int vk)
{
	KEYBDINPUT ki;
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	ki.wVk = vk;
	ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
	this->kiVector.push_back(ki);

	ki.dwFlags |= KEYEVENTF_KEYUP;
	this->kiVector.push_back(ki);
}

void KeyboardVector::mirrorVectorAllUp()
{
	KEYBDINPUT ki;
	for (int i = kiVector.size() - 1; i >= 0; i--)
	{
		ki = kiVector.at(i);
		ki.dwFlags |= KEYEVENTF_KEYUP;
		kiVector.push_back(ki);
	}
}

KeyboardVector KeyboardVector::createFromCommand(kybdCmds cmd)
{
	KeyboardVector keyboardVector;
	for (int i = 1; i < ULONG_MAX / 2; i *= 2)
	{
		// loop through all kybdCmds and input any that show up, squashing them 
		// after each loop.
		switch (kybdCmds(unsigned int(cmd & i)))
		{
		case UNDO:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x5A); // 'Z' key
			break;
		case REDO:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x59); // 'Y' key
			break;
		case ZOOM_IN:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(VK_OEM_PLUS);
			break;
		case ZOOM_OUT:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(VK_OEM_MINUS);
			break;
		case ZOOM_100:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x30); // '0' key
			break;
		case ESCAPE:
			keyboardVector.inputVKDown(VK_ESCAPE);
			break;
		case ENTER:
			keyboardVector.inputVKDown(VK_RETURN);
			break;
		case TAB:
			keyboardVector.inputVKDown(VK_TAB);
			break;
		case SWITCH_WIN_FORWARD:
			// This only works if uiAccess is set to True.
			keyboardVector.inputVKDown(VK_MENU);
			keyboardVector.inputVKDown(VK_TAB);
			break;
		case SWITCH_WIN_REVERSE:
			// This only works if uiAccess is set to True.
			keyboardVector.inputVKDown(VK_MENU);
			keyboardVector.inputVKDown(VK_LSHIFT);
			keyboardVector.inputVKDown(VK_TAB);
			break;
		case COPY:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x43); // 'C' key
			break;
		case PASTE:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x56); // 'V' key
			break;
		case CUT:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x58); // 'X' key
			break;
		case FILE_MENU:
			keyboardVector.inputVKDown(VK_MENU);
			keyboardVector.inputVKDown(0x46); // 'F' key
			break;
		case NEW_BROWSER:
			keyboardVector.inputVKDown(VK_CONTROL);
			keyboardVector.inputVKDown(0x4E); // 'N' key
			break;
		case GOTO_ADDR_BAR:
			keyboardVector.inputVKDown(VK_MENU);
			keyboardVector.inputVKDown(0x44); // 'D' key
			break;
		case LOCK_DESKTOP:
			// THIS ONE IS NOT WORKING TODO
			keyboardVector.inputVKDown(VK_LWIN);
			keyboardVector.inputVKDown(0x4C); // 'L' key
			break;
		case EDIT_MENU:
			keyboardVector.inputVKDown(VK_MENU);
			keyboardVector.inputVKDown(0x45); // 'E' key
			break;
		case VIEW_MENU:
			keyboardVector.inputVKDown(VK_MENU);
			keyboardVector.inputVKDown(0x56); // 'V' key
			break;
		case WIN_HOME:
			keyboardVector.inputVKDown(VK_LWIN);
			break;
		case HIDE_APPS:
			keyboardVector.inputVKDown(VK_LWIN);
			keyboardVector.inputVKDown(0x44); // 'D' key
			break;
		case CONTROL:
			keyboardVector.inputVKDown(VK_CONTROL);
			break;
		case VOLUME_UP:
			keyboardVector.inputVKDown(VK_VOLUME_UP);
			break;
		case VOLUME_DOWN:
			keyboardVector.inputVKDown(VK_VOLUME_DOWN);
			break;
		case BACKSPACE:
			keyboardVector.inputVKDown(VK_BACK);
			break;
		case UP_ARROW:
			keyboardVector.inputVKDown(VK_UP);
			break;
		case RIGHT_ARROW:
			keyboardVector.inputVKDown(VK_RIGHT);
			break;
		case DOWN_ARROW:
			keyboardVector.inputVKDown(VK_DOWN);
			break;
		case LEFT_ARROW:
			keyboardVector.inputVKDown(VK_LEFT);
			break;
		default:
			break;
		}

		cmd = kybdCmds(unsigned int(cmd) & ~i);
	}

	keyboardVector.mirrorVectorAllUp();

	return keyboardVector;
}

KeyboardVector KeyboardVector::createFromProfileStr(std::string str)
{
	KeyboardVector keyboardVector;

	char * cstr = new char[str.length() + 1];
	std::strcpy(cstr, str.c_str());
	char* inputVector = strtok(cstr, ",");
	inputVector = strtok(NULL, ","); // do twice as we want the characters after the (first) comma
	for (int i = 0; inputVector[i] != NULL; i++)
	{
		keyboardVector.inputCharDownUp(inputVector[i]);
	}

	delete[] cstr;
	return keyboardVector;
}

unsigned int KeyboardVector::tryInsertCharAsVK(char c)
{
	int cInt = int(c);
	
	bool isCapital = false;
	if (cInt >= 0x41 && cInt <= 0x5A)
	{
		isCapital = true;
	}
	else if (cInt >= 0x61 && cInt <= 0x7A)
	{
		cInt -= 0x20;
	}
	else
	{
		// return -1 if the mapping is not simple (not being handled right now)
		// only alpha
		return -1;
	}

	if (isCapital)
	{
		this->inputVKDown(VK_SHIFT);
	}
	
	this->inputVKDownUp(cInt);

	if (isCapital)
	{
		this->inputVKUp(VK_SHIFT);
	}
	return 0;
}