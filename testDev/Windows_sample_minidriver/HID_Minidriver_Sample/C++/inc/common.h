/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    common.h

Environment:

    User mode

--*/

#ifndef __VHIDMINI_COMMON_H__
#define __VHIDMINI_COMMON_H__

//
// This is the report id of the collection to which the control codes are sent
//
#define CONTROL_COLLECTION_REPORT_ID                      0x01
#define TEST_COLLECTION_REPORT_ID                         0x02

#define MAXIMUM_STRING_LENGTH           (126 * sizeof(WCHAR))
#define VHIDMINI_DEVICE_STRING          L"UMDF Virtual hidmini device"  
#define VHIDMINI_MANUFACTURER_STRING    L"UMDF Virtual hidmini device Manufacturer string"  
#define VHIDMINI_PRODUCT_STRING         L"UMDF Virtual hidmini device Product string"  
#define VHIDMINI_SERIAL_NUMBER_STRING   L"UMDF Virtual hidmini device Serial Number string"  
#define VHIDMINI_DEVICE_STRING          L"UMDF Virtual hidmini device"  
#define VHIDMINI_DEVICE_STRING_INDEX    5
#include <pshpack1.h>

//
// input from device to system
//
typedef struct _HIDMINI_INPUT_REPORT {
    
    UCHAR ReportId;   
	UINT8 buttons;
	INT8 x;
	INT8 y;

} HIDMINI_INPUT_REPORT, *PHIDMINI_INPUT_REPORT;

#include <poppack.h>

//
// SetFeature request requires that the feature report buffer size be exactly 
// same as the size of report described in the hid report descriptor (
// excluding the report ID). Since HIDMINI_CONTROL_INFO includes report ID,
// we subtract one from the size.
//
#define INPUT_REPORT_SIZE_CB        ((USHORT)(sizeof(HIDMINI_INPUT_REPORT) - 1))

#endif //__VHIDMINI_COMMON_H__
