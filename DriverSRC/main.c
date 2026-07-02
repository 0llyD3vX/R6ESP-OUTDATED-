#include <ntddk.h>
#include <wdm.h>
#include <stdio.h>
#include "structures.h"
#include "offsets.h"
#include "game.h"

// ============================================================
// GLOBALS
// ============================================================
GameData g_GameData = { 0 };
HANDLE g_hSharedMemory = NULL;
BOOLEAN g_DriverRunning = TRUE;
KEVENT g_UpdateEvent;
HANDLE g_hThread = NULL;
PDEVICE_OBJECT g_DeviceObject = NULL;  // Store device object

// ============================================================
// FORWARD DECLARATIONS
// ============================================================
NTSTATUS DriverCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DriverClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DriverIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID DriverUnload(PDRIVER_OBJECT DriverObject);
VOID UpdateThread(PVOID Context);
uint32_t GetGamePid();

// ============================================================
// DRIVER ENTRY
// ============================================================
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    NTSTATUS status;
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\RainbowSixESP");
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\RainbowSixESP");
    PDEVICE_OBJECT deviceObject = NULL;

    DbgPrint("[RainbowSixESP] Driver Loading...\n");

    // 1. Create device
    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) {
        DbgPrint("[RainbowSixESP] Failed to create device: 0x%X\n", status);
        return status;
    }
    g_DeviceObject = deviceObject;

    // 2. Create symbolic link
    status = IoCreateSymbolicLink(&symLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        DbgPrint("[RainbowSixESP] Failed to create symlink: 0x%X\n", status);
        return status;
    }

    // 3. Setup dispatch routines
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverIoctl;
    DriverObject->DriverUnload = DriverUnload;

    // 4. Initialize shared memory
    status = InitializeSharedMemory();
    if (!NT_SUCCESS(status)) {
        DbgPrint("[RainbowSixESP] Failed to init shared memory: 0x%X\n", status);
        IoDeleteSymbolicLink(&symLink);
        IoDeleteDevice(deviceObject);
        return status;
    }

    // 5. Initialize update event
    KeInitializeEvent(&g_UpdateEvent, NotificationEvent, FALSE);

    // 6. Start update thread
    status = PsCreateSystemThread(&g_hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, UpdateThread, NULL);
    if (!NT_SUCCESS(status)) {
        DbgPrint("[RainbowSixESP] Failed to create thread: 0x%X\n", status);
        IoDeleteSymbolicLink(&symLink);
        IoDeleteDevice(deviceObject);
        return status;
    }

    DbgPrint("[RainbowSixESP] Driver Loaded Successfully!\n");
    return STATUS_SUCCESS;
}

// ============================================================
// DRIVER UNLOAD - PROPER CLEANUP
// ============================================================
VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\RainbowSixESP");
    LARGE_INTEGER timeout;
    timeout.QuadPart = -30000000;  // 3 seconds

    DbgPrint("[RainbowSixESP] Driver Unloading...\n");

    // 1. Stop the driver
    g_DriverRunning = FALSE;

    // 2. Signal the event to wake up the thread
    KeSetEvent(&g_UpdateEvent, IO_NO_INCREMENT, FALSE);

    // 3. Wait for thread to exit (don't force terminate)
    if (g_hThread) {
        DbgPrint("[RainbowSixESP] Waiting for thread to exit...\n");
        KeWaitForSingleObject(g_hThread, Executive, KernelMode, FALSE, &timeout);

        // Close the thread handle
        ZwClose(g_hThread);
        g_hThread = NULL;
    }

    // 4. Cleanup shared memory
    DbgPrint("[RainbowSixESP] Cleaning up shared memory...\n");
    CleanupSharedMemory();

    // 5. Delete symbolic link and device
    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(DriverObject->DeviceObject);

    DbgPrint("[RainbowSixESP] Driver Unloaded Successfully!\n");
}

// ============================================================
// DISPATCH ROUTINES
// ============================================================
NTSTATUS DriverCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DriverClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DriverIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG ioctlCode = stack->Parameters.DeviceIoControl.IoControlCode;
    ULONG outputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;
    PVOID outputBuffer = Irp->AssociatedIrp.SystemBuffer;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG bytesReturned = 0;

    __try {
        switch (ioctlCode) {
        case IOCTL_GET_GAME_DATA:
            if (outputLength >= sizeof(GameData)) {
                // Use a local copy to avoid race conditions
                GameData localData;
                RtlCopyMemory(&localData, &g_GameData, sizeof(GameData));
                RtlCopyMemory(outputBuffer, &localData, sizeof(GameData));
                bytesReturned = sizeof(GameData);
                status = STATUS_SUCCESS;
            }
            else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;

        case IOCTL_GET_STATUS: {
            DriverStatus* statusData = (DriverStatus*)outputBuffer;
            if (outputLength >= sizeof(DriverStatus)) {
                statusData->status = STATUS_DRIVER_LOADED;
                statusData->gamePid = GetGamePid();
                statusData->timestamp = GetCurrentTimestamp();
                bytesReturned = sizeof(DriverStatus);
                status = STATUS_SUCCESS;
            }
            else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;
        }

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = STATUS_UNSUCCESSFUL;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytesReturned;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

// ============================================================
// UPDATE THREAD
// ============================================================
VOID UpdateThread(PVOID Context) {
    LARGE_INTEGER timeout;
    timeout.QuadPart = -10000;  // 1ms

    DbgPrint("[RainbowSixESP] Update Thread Started\n");

    while (g_DriverRunning) {
        // Read game data
        ReadGameData(&g_GameData);

        // Wait for next update or signal to exit
        KeWaitForSingleObject(&g_UpdateEvent, Executive, KernelMode, FALSE, &timeout);
    }

    DbgPrint("[RainbowSixESP] Update Thread Exiting\n");
    PsTerminateSystemThread(STATUS_SUCCESS);
}

// ============================================================
// UTILITY FUNCTIONS
// ============================================================
uint32_t GetGamePid() {
    PEPROCESS process = PsGetCurrentProcess();
    return (uint32_t)PsGetProcessId(process);
}