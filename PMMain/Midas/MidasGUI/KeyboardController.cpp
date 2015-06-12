#include "KeyboardContoller.h"

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