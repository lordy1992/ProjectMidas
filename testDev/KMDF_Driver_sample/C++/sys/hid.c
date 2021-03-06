// J^2 - Checked file for now.

/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    hid.c

Abstract:

    Code for handling HID related requests

Author:


Environment:

    kernel mode only

Revision History:

--*/

#define TEMPORARY
#define HARDCODED_HID_REPORT_DESCRIPTOR

#include <hidusbfx2.h>

#if defined(EVENT_TRACING)
#include "hid.tmh"
#endif

#ifdef ALLOC_PRAGMA
    //#pragma alloc_text( PAGE, HidFx2SetFeature)
    //#pragma alloc_text( PAGE, HidFx2GetFeature)
#endif

/*++

Routine Description:

This event is called when the framework receives
IRP_MJ_INTERNAL DEVICE_CONTROL requests from the system.

Arguments:

Queue - Handle to the framework queue object that is associated
with the I/O request.
Request - Handle to a framework request object.

OutputBufferLength - length of the request's output buffer,
if an output buffer is available.
InputBufferLength - length of the request's input buffer,
if an input buffer is available.

IoControlCode - the driver-defined or system-defined I/O control code
(IOCTL) that is associated with the request.
Return Value:

VOID

--*/
VOID
HidFx2EvtInternalDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
    )
{
    NTSTATUS            status = STATUS_SUCCESS;
    WDFDEVICE           device;
    PDEVICE_EXTENSION   devContext = NULL;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    devContext = GetDeviceContext(device);

	DbgPrint("Debugging Ioctl: %s", DbgHidInternalIoctlString(IoControlCode));
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL,
        "%s, Queue:0x%p, Request:0x%p\n",
        DbgHidInternalIoctlString(IoControlCode),
        Queue, 
        Request
        );

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    switch(IoControlCode) {

    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        //
        // Retrieves the device's HID descriptor.
        //
        status = HidFx2GetHidDescriptor(device, Request);
        break;

    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        //
        //Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
        //
        status = HidFx2GetDeviceAttributes(Request);
        break;

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        //
        //Obtains the report descriptor for the HID device.
        //
        status = HidFx2GetReportDescriptor(device, Request);
        break;

    case IOCTL_HID_READ_REPORT:

        //
        // Returns a report from the device into a class driver-supplied buffer.
        // For now queue the request to the manual queue. The request will
        // be retrived and completd when continuous reader reads new data
        // from the device.
        //
        status = WdfRequestForwardToIoQueue(Request, devContext->InterruptMsgQueue);

        if(!NT_SUCCESS(status)){
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
                "WdfRequestForwardToIoQueue failed with status: 0x%x\n", status);
            
            WdfRequestComplete(Request, status);
        }

        return;

//
// This feature is only supported on WinXp and later. Compiling in W2K 
// build environment will fail without this conditional preprocessor statement.
//
    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:

		DbgPrint("Hit IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST... Didn't expect to get here.");
        
        return;

    case IOCTL_HID_SET_FEATURE:
        //
        // This sends a HID class feature report to a top-level collection of
        // a HID class device.
        //
		DbgPrint("Hit IOCTL_HID_SET_FEATURE... Didn't expect to get here.");
        return;
        
    case IOCTL_HID_GET_FEATURE:
        //
        // Get a HID class feature report from a top-level collection of
        // a HID class device.
        //
		DbgPrint("Hit IOCTL_HID_GET_FEATURE... Didn't expect to get here.");
        return;

	case IOCTL_HID_WRITE_REPORT:
        //
        //Transmits a class driver-supplied report to the device.
        //
    case IOCTL_HID_GET_STRING:
        //
        // Requests that the HID minidriver retrieve a human-readable string
        // for either the manufacturer ID, the product ID, or the serial number
        // from the string descriptor of the device. The minidriver must send
        // a Get String Descriptor request to the device, in order to retrieve
        // the string descriptor, then it must extract the string at the
        // appropriate index from the string descriptor and return it in the
        // output buffer indicated by the IRP. Before sending the Get String
        // Descriptor request, the minidriver must retrieve the appropriate
        // index for the manufacturer ID, the product ID or the serial number
        // from the device extension of a top level collection associated with
        // the device.
        //
    case IOCTL_HID_ACTIVATE_DEVICE:
        //
        // Makes the device ready for I/O operations.
        //
    case IOCTL_HID_DEACTIVATE_DEVICE:
        //
        // Causes the device to cease operations and terminate all outstanding
        // I/O requests.
        //
    default:
		DbgPrint("Hit Not supported...");
        status = STATUS_NOT_SUPPORTED;
        break;
    }

    WdfRequestComplete(Request, status);

    return;
}

#ifndef TEMPORARY
NTSTATUS
HidFx2SetFeature(
    IN WDFREQUEST Request
    )
/*++

Routine Description

    This routine sets the state of the Feature: in this
    case Segment Display on the USB FX2 board.

Arguments:

    Request - Wdf Request 

Return Value:

    NT status value

--*/
{
    NTSTATUS                     status = STATUS_SUCCESS;
    PHID_XFER_PACKET             transferPacket = NULL;
    WDF_REQUEST_PARAMETERS       params;
    PHIDFX2_FEATURE_REPORT       featureReport = NULL;
    WDFDEVICE                    device;
    UCHAR                        featureUsage = 0;

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "HidFx2SetFeature Enter\n");

    device = WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request));
    
    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    //
    // IOCTL_HID_SET_FEATURE & IOCTL_HID_GET_FEATURE are not METHOD_NIEHTER
    // IOCTLs. So you cannot retreive UserBuffer from the IRP using Wdf
    // function. As a result we have to escape out to WDM to get the UserBuffer
    // directly from the IRP. 
    //
    if (params.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "Userbuffer is small 0x%x\n", status);
        return status;
    }

    //
    // This is a kernel buffer so no need for try/except block when accesssing
    // Irp->UserBuffer.
    //
    transferPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (transferPacket == NULL) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "Irp->UserBuffer is NULL 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen == 0){
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "HID_XFER_PACKET->reportBufferLen is 0, 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen < sizeof(UCHAR)){
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "HID_XFER_PACKET->reportBufferLen is too small, 0x%x\n", status);
        return status;
    }

    featureReport = (PHIDFX2_FEATURE_REPORT)transferPacket->reportBuffer;
    featureUsage = featureReport->FeatureData;

	//
	// The feature reports map directly to the command
	// data that is sent down to the device.
	//
	if (transferPacket->reportId == 'a')
	{
		DbgPrint("Report ID...");
		return STATUS_INVALID_DEVICE_REQUEST;
	}
	else if (transferPacket->reportId == BARGRAPH_REPORT_ID)
	{
        status = SendVendorCommand(
            device, 
            HIDFX2_SET_BARGRAPH_DISPLAY,
            &featureUsage
            );
	}
	else
	{
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "Incorrect report ID, 0x%x\n", status);
        return status;
	}

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "HidFx2SetFeature Exit\n");
    return status;
}

NTSTATUS
HidFx2GetFeature(
    IN WDFREQUEST Request,
	OUT PULONG BytesReturned
    )
/*++

Routine Description

    This routine gets the state of the Feature: in this
    case Segment Display or bargraph display on the USB FX2 board.

Arguments:

    Request - Wdf Request 

	BytesReturned - Size of buffer returned for the request

Return Value:

    NT status value

--*/
{
    NTSTATUS                     status = STATUS_SUCCESS;
    PHID_XFER_PACKET             transferPacket = NULL;
    WDF_REQUEST_PARAMETERS       params;
    PHIDFX2_FEATURE_REPORT       featureReport = NULL;
    WDFDEVICE                    device;

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "HidFx2GetFeature Enter\n");

    *BytesReturned = 0;

    device = WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request));
    
    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    //
    // IOCTL_HID_SET_FEATURE & IOCTL_HID_GET_FEATURE are not METHOD_NIEHTER
    // IOCTLs. So you cannot retreive UserBuffer from the IRP using Wdf
    // function. As a result we have to escape out to WDM to get the UserBuffer
    // directly from the IRP. 
    //
    if (params.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "Userbuffer is small 0x%x\n", status);
        return status;
    }

    //
    // This is a kernel buffer so no need for try/except block when accesssing
    // Irp->UserBuffer.
    //
    transferPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (transferPacket == NULL) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "Irp->UserBuffer is NULL 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen == 0){
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "HID_XFER_PACKET->reportBufferLen is 0, 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen < sizeof(UCHAR)){
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "HID_XFER_PACKET->reportBufferLen is too small, 0x%x\n", status);
        return status;
    }

    featureReport = (PHIDFX2_FEATURE_REPORT)transferPacket->reportBuffer;

    if (transferPacket->reportId == SEVEN_SEGMENT_REPORT_ID)
    {
        status = GetVendorData(
            device, 
            HIDFX2_READ_7SEGMENT_DISPLAY,
            &featureReport->FeatureData 
            );
    }
    else if (transferPacket->reportId == BARGRAPH_REPORT_ID)
    {
        status = GetVendorData(
            device, 
            HIDFX2_READ_BARGRAPH_DISPLAY,
            &featureReport->FeatureData
            );
    }
    else
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "Incorrect report ID, 0x%x\n", status);
        return status;
    }

    *BytesReturned = sizeof (HIDFX2_FEATURE_REPORT);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "HidFx2GetFeature Exit\n");
    return status;
}

NTSTATUS
SendVendorCommand(
    IN WDFDEVICE Device,
    IN UCHAR VendorCommand,
    IN PUCHAR CommandData
    )
/*++

Routine Description

    This routine sets the state of the Feature: in this
    case Segment Display on the USB FX2 board.

Arguments:

    Request - Wdf Request 

Return Value:

    NT status value

--*/
{
    NTSTATUS                     status = STATUS_SUCCESS;
    ULONG                        bytesTransferred =0;
    PDEVICE_EXTENSION            pDevContext = NULL;
    WDF_MEMORY_DESCRIPTOR        memDesc;
    WDF_USB_CONTROL_SETUP_PACKET controlSetupPacket;
    WDF_REQUEST_SEND_OPTIONS     sendOptions;
    
    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "SendVendorCommand Enter\n");

    pDevContext = GetDeviceContext(Device);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL,
        "SendVendorCommand: Command:0x%x, data: 0x%x\n", 
        VendorCommand, *CommandData);

    //
    // set the segment state on the USB device
    //
    // Send the I/O with a timeout. We do that because we send the
    // I/O in the context of the user thread and if it gets stuck, it would
    // prevent the user process from existing. 
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions,
                                  WDF_REQUEST_SEND_OPTION_TIMEOUT);

    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&sendOptions,
                                         WDF_REL_TIMEOUT_IN_SEC(5));

    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                        BmRequestHostToDevice,
                                        BmRequestToDevice,
                                        VendorCommand, // Request
                                        0, // Value
                                        0); // Index

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc,
                                    CommandData,
                                    sizeof(UCHAR));

    status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                                pDevContext->UsbDevice,
                                                WDF_NO_HANDLE, // Optional WDFREQUEST
                                                &sendOptions, // PWDF_REQUEST_SEND_OPTIONS
                                                &controlSetupPacket,
                                                &memDesc,
                                                &bytesTransferred
                                                );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "SendtVendorCommand: Failed to set Segment Display state - 0x%x \n",
            status);
    } 

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "SendVendorCommand Exit\n");

    return status;
}

NTSTATUS
GetVendorData(
    IN WDFDEVICE Device,
    IN UCHAR VendorCommand,
    IN PUCHAR CommandData
    )
/*++

Routine Description

    This routine sets the state of the Feature: in this
    case Segment Display on the USB FX2 board.

Arguments:

    Request - Wdf Request 

Return Value:

    NT status value

--*/
{
    NTSTATUS                     status = STATUS_SUCCESS;
    ULONG                        bytesTransferred =0;
    PDEVICE_EXTENSION            pDevContext = NULL;
    WDF_MEMORY_DESCRIPTOR        memDesc;
    WDF_USB_CONTROL_SETUP_PACKET controlSetupPacket;
    WDF_REQUEST_SEND_OPTIONS     sendOptions;
    
    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "GetVendorData Enter\n");

    pDevContext = GetDeviceContext(Device);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL,
        "GetVendorData: Command:0x%x, data: 0x%x\n", 
        VendorCommand, *CommandData);

    //
    // Get the display state from the USB device
    //
    // Send the I/O with a timeout. We do that because we send the
    // I/O in the context of the user thread and if it gets stuck, it would
    // prevent the user process from existing. 
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions,
                                  WDF_REQUEST_SEND_OPTION_TIMEOUT);

    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&sendOptions,
                                         WDF_REL_TIMEOUT_IN_SEC(5));

    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                        BmRequestDeviceToHost,
                                        BmRequestToDevice,
                                        VendorCommand, // Request
                                        0, // Value
                                        0); // Index

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc,
                                    CommandData,
                                    sizeof(UCHAR));

    status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                                pDevContext->UsbDevice,
                                                WDF_NO_HANDLE, // Optional WDFREQUEST
                                                &sendOptions, // PWDF_REQUEST_SEND_OPTIONS
                                                &controlSetupPacket,
                                                &memDesc,
                                                &bytesTransferred
                                                );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "GetVendorData: Failed to get state - 0x%x \n",
            status);
    } 
    else
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL,
            "GetVendorData: Command:0x%x, data after command: 0x%x\n", 
            VendorCommand, *CommandData);
    }

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "GetVendorData Exit\n");

    return status;
} 
#endif


NTSTATUS
HidFx2GetHidDescriptor(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
    )
/*++

Routine Description:

    Finds the HID descriptor and copies it into the buffer provided by the 
    Request.

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    size_t              bytesToCopy = 0;
    WDFMEMORY           memory;

    UNREFERENCED_PARAMETER(Device);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
        "HidFx2GetHidDescriptor Entry\n");

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "WdfRequestRetrieveOutputMemory failed 0x%x\n", status);
        return status;
    }

    //
    // Use hardcoded "HID Descriptor" 
    //
    bytesToCopy = G_DefaultHidDescriptor.bLength;

    if (bytesToCopy == 0) {
        status = STATUS_INVALID_DEVICE_STATE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "G_DefaultHidDescriptor is zero, 0x%x\n", status);
        return status;        
    }
    
    status = WdfMemoryCopyFromBuffer(memory,
                            0, // Offset
                            (PVOID) &G_DefaultHidDescriptor,
                            bytesToCopy);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
        "HidFx2GetHidDescriptor Exit = 0x%x\n", status);
    return status;
}

NTSTATUS
HidFx2GetReportDescriptor(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
    )
/*++

Routine Description:

    Finds the Report descriptor and copies it into the buffer provided by the
    Request.

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    ULONG_PTR           bytesToCopy;
    WDFMEMORY           memory;

    UNREFERENCED_PARAMETER(Device);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
        "HidFx2GetReportDescriptor Entry\n");

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "WdfRequestRetrieveOutputMemory failed 0x%x\n", status);
        return status;
    }

    //
    // Use hardcoded Report descriptor
    //
    bytesToCopy = G_DefaultHidDescriptor.DescriptorList[0].wReportLength;

    if (bytesToCopy == 0) {
        status = STATUS_INVALID_DEVICE_STATE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "G_DefaultHidDescriptor's reportLenght is zero, 0x%x\n", status);
        return status;        
    }
    
    status = WdfMemoryCopyFromBuffer(memory,
                            0,
                            (PVOID) G_DefaultReportDescriptor,
                            bytesToCopy);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
        "HidFx2GetReportDescriptor Exit = 0x%x\n", status);
    return status;
}


NTSTATUS
HidFx2GetDeviceAttributes(
    IN WDFREQUEST Request
    )
/*++

Routine Description:

    Fill in the given struct _HID_DEVICE_ATTRIBUTES

Arguments:

    Request - Pointer to Request object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                 status = STATUS_SUCCESS;
    PHID_DEVICE_ATTRIBUTES   deviceAttributes = NULL;
    PDEVICE_EXTENSION        deviceInfo = NULL;

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
        "HidFx2GetDeviceAttributes Entry\n");

    deviceInfo = GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request)));

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputBuffer(Request,
                                            sizeof (HID_DEVICE_ATTRIBUTES),
                                            &deviceAttributes,
                                            NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
            "WdfRequestRetrieveOutputBuffer failed 0x%x\n", status);
        return status;
    }

    deviceAttributes->Size = sizeof (HID_DEVICE_ATTRIBUTES);
	deviceAttributes->VendorID = 0x4A4C; //JL
	deviceAttributes->ProductID = 0x4A48; // JH
	deviceAttributes->VersionNumber = 0x4A4A; // JJ

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, sizeof (HID_DEVICE_ATTRIBUTES));

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
        "HidFx2GetDeviceAttributes Exit = 0x%x\n", status);
    return status;
}

PCHAR
DbgHidInternalIoctlString(
    IN ULONG        IoControlCode
    )
/*++

Routine Description:

    Returns Ioctl string helpful for debugging

Arguments:

    IoControlCode - IO Control code

Return Value:

    Name String

--*/
{
    switch (IoControlCode)
    {
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        return "IOCTL_HID_GET_DEVICE_DESCRIPTOR";
    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        return "IOCTL_HID_GET_REPORT_DESCRIPTOR";
    case IOCTL_HID_READ_REPORT:
        return "IOCTL_HID_READ_REPORT";
    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        return "IOCTL_HID_GET_DEVICE_ATTRIBUTES";
    case IOCTL_HID_WRITE_REPORT:
        return "IOCTL_HID_WRITE_REPORT";
    case IOCTL_HID_SET_FEATURE:
        return "IOCTL_HID_SET_FEATURE";
    case IOCTL_HID_GET_FEATURE:
        return "IOCTL_HID_GET_FEATURE";
    case IOCTL_HID_GET_STRING:
        return "IOCTL_HID_GET_STRING";
    case IOCTL_HID_ACTIVATE_DEVICE:
        return "IOCTL_HID_ACTIVATE_DEVICE";
    case IOCTL_HID_DEACTIVATE_DEVICE:
        return "IOCTL_HID_DEACTIVATE_DEVICE";
    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
        return "IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST";
    default:
        return "Unknown IOCTL";
    }
}


