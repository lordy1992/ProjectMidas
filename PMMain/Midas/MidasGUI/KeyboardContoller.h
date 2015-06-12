#ifndef _KEYBOARD_CONTROLLER_H
#define _KEYBOARD_CONTROLLER_H

#include <Windows.h>
#include "MidasCommon.h"
#include "KeyboardVector.h"

/**
* Handles sending keyboard command data to Windows OS.
*/
class KeyboardController
{
public:
	/* Basic Construction/Destruction */
	KeyboardController();
	~KeyboardController();

	void setKiVector(KeyboardVector kiVector) { this->kiVector = kiVector; }
	KeyboardVector getKiVector() { return this->kiVector; }

	int sendData();
	int sendDataDelayed(int milliDelay);

private:
	
	INPUT* convertKiVecToInputArr(KeyboardVector& kiVector);

	KeyboardVector kiVector;
};


#endif /* _KEYBOARD_CONTROLLER_H */