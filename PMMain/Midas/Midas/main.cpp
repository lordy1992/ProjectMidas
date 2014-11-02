#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"

using namespace std;

int main(void) {
	// MANUAL TEST 1
	cout << "hijacking test main..." << endl;

	kybrdCtrl * controller = new kybrdCtrl();
	int count = 0;
	while (true) {
		if (count++ % 2 == 0) {
			controller->setKeyCmd(kybdCmds::ZOOM_IN);
			//controller->setKeyChar('a');
		}
		else {
			controller->setKeyCmd(kybdCmds::ZOOM_OUT);
			//controller->setKeyChar('b');
		}
		
		int status = controller->sendData();
		cout << "looping with opposite commands... status = " << status << endl;

		Sleep(1000);
	}

	system("PAUSE");
	return 0;
}