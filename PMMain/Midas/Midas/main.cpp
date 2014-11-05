#include <iostream>
#include <string>
#include <Windows.h>
#include "kybrdCtrl.h"
#include "WearableDevice.h"
#include <vector>
#include <thread>
#include "SharedCommandData.h"

#include "SharedCommandDataTest.h"
#include "KybrdCtrlTest.h"

using namespace std;

#define KEYBOARD_CONTROL_TEST

int main() {

#ifdef TEST_WEARABLE_DEVICE
    SharedCommandData_test::testQueue();
#endif

#ifdef KEYBOARD_CONTROL_TEST
    KybrdCtrlTest::testLock();
    KybrdCtrlTest::testAltTab();
#endif

    system("PAUSE");
    return 0;
}
 


