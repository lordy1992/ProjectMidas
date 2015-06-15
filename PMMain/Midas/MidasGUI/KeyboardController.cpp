#include "KeyboardContoller.h"
#include "KeyboardVector.h"

INPUT* KeyboardController::convertKiVecToInputArr(KeyboardVector& kiVector)
{
	INPUT* kiArr = (INPUT*)std::malloc(sizeof(INPUT) * kiVector.peakVector().size());

	for (unsigned int i = 0; i < kiVector.peakVector().size(); i++)
	{
		kiArr[i].type = INPUT_KEYBOARD;
		kiArr[i].ki = kiVector.peakVector().at(i);
	}

	return kiArr;
}

int KeyboardController::sendData()
{
	INPUT* kiArr = convertKiVecToInputArr(kiVector);

	int numSent = SendInput(kiVector.peakVector().size(), kiArr, sizeof(INPUT));

	free(kiArr);

	/* TEMP CODE TO TEST VIRTUAL KEY INTERACTION WITH GLOBE - Jorden TODO Remove*/

	//KeyboardVector kv;
	//kv.inputVKDown(0x10);
	//kv.inputVKDownUp(0x5A); //Z
	//kv.inputVKUp(0x10);
	//kiArr = convertKiVecToInputArr(kv);
	//numSent = SendInput(kv.peakVector().size(), kiArr, sizeof(INPUT));
	//free(kiArr);

	//INPUT kiArr2[2];
	//kiArr2[0].type = INPUT_KEYBOARD;
	//kiArr2[1].type = INPUT_KEYBOARD;
	//KEYBDINPUT kybdInput0;
	//KEYBDINPUT kybdInput1;
	//
	//ZeroMemory(&kybdInput0, sizeof(KEYBDINPUT));
	//ZeroMemory(&kybdInput1, sizeof(KEYBDINPUT));ABCZz
	//int cInt = int('Z');
	//kybdInput0.wVk = 0;
	//kybdInput0.wScan = cInt;
	//kybdInput0.dwFlags = KEYEVENTF_UNICODE;
	//kiArr2[0].ki = kybdInput0;
	//
	//kybdInput1 = kybdInput0;
	//kybdInput1.dwFlags |= KEYEVENTF_KEYUP;
	//kiArr2[1].ki = kybdInput1;
	//
	//SendInput(2, kiArr2, sizeof(INPUT));
	//
	//INPUT kiArr3[4];
	//kiArr3[0].type = INPUT_KEYBOARD;
	//kiArr3[1].type = INPUT_KEYBOARD;
	//kiArr3[2].type = INPUT_KEYBOARD;
	//kiArr3[3].type = INPUT_KEYBOARD;
	//
	//ZeroMemory(&kybdInput0, sizeof(KEYBDINPUT));
	//ZeroMemory(&kybdInput1, sizeof(KEYBDINPUT));ABC
	//kybdInput0.wVk = 0x5A;
	//kybdInput0.wScan = 0;
	//kybdInput0.dwFlags = 0;
	//kiArr3[0].ki = kybdInput0;
	//
	//kybdInput1 = kybdInput0;
	//kybdInput1.dwFlags |= KEYEVENTF_KEYUP;
	//kiArr3[1].ki = kybdInput1;
	//
	//SendInput(2, kiArr3, sizeof(INPUT));

	return numSent;
}

int KeyboardController::sendDataDelayed(int milliDelay)
{
	INPUT* kiArr = convertKiVecToInputArr(kiVector);

	int numSent = 0;
	for (int i = 0; i < kiVector.peakVector().size(); i++)
	{
		numSent += SendInput(1, kiArr + i, sizeof(INPUT));
		Sleep(milliDelay);
	}

	free(kiArr);

	return numSent;
}