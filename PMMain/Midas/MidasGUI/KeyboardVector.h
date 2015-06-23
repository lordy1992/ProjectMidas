#pragma once
#include <vector>
#include <Windows.h>
#include "MidasCommon.h"

#define NO_VK_REP_FOUND -1

class KeyboardVector {
public:
	void clear();

	void inputCharDown(char c);

	void inputCharUp(char c);

	void inputCharDownUp(char c);

	void inputVKDown(unsigned int vk);

	void inputVKUp(unsigned int vk);

	void inputVKDownUp(unsigned int vk);

	void mirrorVectorAllUp();

	const std::vector<KEYBDINPUT> peakVector() { return kiVector; }

	static KeyboardVector createFromCommand(kybdCmds cmd);

	static KeyboardVector createFromProfileStr(std::string str);

private:
	unsigned int tryInsertCharAsVK(char c);

	std::vector<KEYBDINPUT> kiVector;
};