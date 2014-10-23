/*++
Copyright (c) 2008  Microsoft Corporation

Module Name:

    moufiltr.h

Abstract:

    This module contains the common private declarations for the mouse
    packet filter

Environment:

    kernel mode only

Notes:


Revision History:


--*/

#ifndef MOUFILTER_H
#define MOUFILTER_H

#include <ntddk.h>
#include <kbdmou.h>
#include <ntddmou.h>
#include <ntdd8042.h>
#include <wdf.h>

#if DBG

#define TRAP() DbgBreakPoint()
#define DebugPrint(_x_) DbgPrint _x_

#else   // DBG

#define TRAP()
#define DebugPrint(_x_)

#endif

typedef struct _DEVICE_EXTENSION
{                              
    PVOID UpperContext;

    //
    // Context for QueueMousePacket
    //
    IN PVOID CallContext;

    //
    // The real connect data that this driver reports to
    //
    CONNECT_DATA UpperConnectData;

  
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _MOUSE_REPORT
{
	UCHAR ReportId;
	SHORT xData;
	SHORT yData;
} MOUSE_REPORT, *PMOUSE_REPORT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION,
                                        FilterGetData)
 
//
// Prototypes
//
DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD MouFilter_EvtDeviceAdd;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL MouFilter_EvtIoInternalDeviceControl;
EVT_WDF_IO_QUEUE_IO_READ MouFilter_IoRead;

VOID MouFilter_DispatchPassThrough(_In_ WDFREQUEST Request, _In_ WDFIOTARGET Target);

VOID MouFilter_ServiceCallback(IN PDEVICE_OBJECT DeviceObject, IN PMOUSE_INPUT_DATA InputDataStart, 
	IN PMOUSE_INPUT_DATA InputDataEnd, IN OUT PULONG InputDataConsumed);

#endif  // MOUFILTER_H


