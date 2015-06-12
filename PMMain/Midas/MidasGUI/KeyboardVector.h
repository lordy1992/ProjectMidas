#pragma once
#include <vector>
#include <Windows.h>
#include "MidasCommon.h"

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

private:
	std::vector<KEYBDINPUT> kiVector;
};