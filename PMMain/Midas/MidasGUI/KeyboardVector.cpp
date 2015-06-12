#include "KeyboardVector.h"
#include <vector>
#include <Windows.h>


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
	KEYBDINPUT ki;
	ZeroMemory(&ki, sizeof(KEYBDINPUT));

	int cInt = int(c);

	ki.wScan = cInt;
	ki.dwFlags = KEYEVENTF_UNICODE;
	this->kiVector.push_back(ki);

	ki.dwFlags |= KEYEVENTF_KEYUP;
	this->kiVector.push_back(ki);
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


