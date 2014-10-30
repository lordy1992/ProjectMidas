/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    usb.c

Abstract:

    Code for handling USB related requests

Author:


Environment:

    kernel mode only

Revision History:

--*/

#include <hidusbfx2.h>

#if defined(EVENT_TRACING)
#include "usb.tmh"
#endif

// Recurring Mouse timer
VOID HidMouseEvtTimerFunction(IN WDFTIMER Timer) {
	HidFx2CompleteReadReport(WdfTimerGetParentObject(Timer));
}


//TODO: This is likely where the actual setting of x/y mouse variables will take place
// done so by reading a report, then manipulating the values at the provided memory locations of the report
VOID
HidFx2CompleteReadReport(
    WDFDEVICE Device
    )
/*++

Routine Description

    This method handles the completion of the pended request for the
    IOCTL_HID_READ_REPORT

Arguments:

    Device - Handle to a framework device.

Return Value:

    None.

--*/
{
    NTSTATUS             status = STATUS_SUCCESS;
    WDFREQUEST           request;
    PDEVICE_EXTENSION    pDevContext = NULL;
    size_t               bytesReturned = 0;
    ULONG                bytesToCopy = 0;
    PHIDFX2_INPUT_REPORT inputReport = NULL;

    pDevContext = GetDeviceContext(Device);

    //
    // Check if there are any pending requests in the Interrupt Message Queue.
    // If a request is found then complete the pending request.
    //
    status = WdfIoQueueRetrieveNextRequest(pDevContext->InterruptMsgQueue, &request);

    if (NT_SUCCESS(status)) {
        //
        // IOCTL_HID_READ_REPORT is METHOD_NEITHER so WdfRequestRetrieveOutputBuffer
        // will correctly retrieve buffer from Irp->UserBuffer. Remember that
        // HIDCLASS provides the buffer in the Irp->UserBuffer field
        // irrespective of the ioctl buffer type. However, framework is very
        // strict about type checking. You cannot get Irp->UserBuffer by using
        // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
        // internal ioctl.
        //
        bytesToCopy = sizeof(HIDFX2_INPUT_REPORT);
        status = WdfRequestRetrieveOutputBuffer(request,
                                                bytesToCopy,
                                                &inputReport,
                                                &bytesReturned);// BufferLength

        if (!NT_SUCCESS(status)) {
            /*TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
                "WdfRequestRetrieveOutputBuffer failed with status: 0x%x\n", status);*/
        } else {

			inputReport->buttons = 0;
			inputReport->ReportId = CONTROL_FEATURE_REPORT_ID;
			inputReport->x = 255;
			inputReport->y = 255;

            bytesReturned = bytesToCopy;

        }

        WdfRequestCompleteWithInformation(request, status, bytesReturned);

    } else if (status != STATUS_NO_MORE_ENTRIES) {
        /*TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "WdfIoQueueRetrieveNextRequest status %08x\n", status);*/
    }

    return;
}
