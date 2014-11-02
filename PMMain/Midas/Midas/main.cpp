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
		INPUT ki_arr[10];
		controller->getKeyInputArr(ki_arr, 10);
		SendInput(controller->getKeyInputVec().size(), ki_arr, sizeof(INPUT));
		cout << "looping with opposite commands..." << endl;

		Sleep(1000);
	}


	// MANUAL TEST 2
	cout << "Running Midas Input Control, please use 'w'/'a'/'s'/'d' to control mouse movement, or \"exit\" to leave." << endl;

	int scale = 20;
	string input;
	MOUSEINPUT mi;
	INPUT mi_arr[2];
	while (true) {
		cin >> input;
		if (input == "exit") {
			break;
		}

		ZeroMemory(&mi, sizeof(MOUSEINPUT));
		mi.dwFlags = MOUSEEVENTF_MOVE;
		if (input == "a") {
			mi.dx = -1;
			mi.dy = 0;
		}
		else if (input == "s") {
			mi.dx = 0;
			mi.dy = 1;
		}
		else if (input == "d") {
			mi.dx = 1;
			mi.dy = 0;
		}
		else if (input == "w") {
			mi.dx = 0;
			mi.dy = -1;
		}
		else if (input == "cl") {
			mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			mi_arr[0].type = INPUT_MOUSE;
			mi_arr[0].mi = mi;
			mi.dwFlags = MOUSEEVENTF_LEFTUP;
			mi_arr[1].type = INPUT_MOUSE;
			mi_arr[1].mi = mi;
		}

		if (input != "cl") {
			mi.dx *= scale;
			mi.dy *= scale;
			mi_arr[0].type = INPUT_MOUSE;
			mi_arr[0].mi = mi;
			SendInput(1, mi_arr, sizeof(INPUT));
		}
		else {
			SendInput(2, mi_arr, sizeof(INPUT));
		}
	}

	system("PAUSE");
	return 0;
}