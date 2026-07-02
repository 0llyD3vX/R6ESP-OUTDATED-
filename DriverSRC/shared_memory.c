#pragma once
#include <ntddk.h>
#include <wdm.h>
#include "structures.h"
#include "game.h"

// ============================================================
// SHARED MEMORY MANAGEMENT
// ============================================================

HANDLE g_hSection = NULL;
PVOID g_pSharedMemory = NULL;
UNICODE_STRING g_sectionName = RTL_CONSTANT_STRING(L"\\BaseNamedObjects\\RainbowSixESP_SharedMemory");

// Initialize shared memory
NTSTATUS InitializeSharedMemory() {
    NTSTATUS status;
    OBJECT_ATTRIBUTES objAttr;
    InitializeObjectAttributes(&objAttr, &g_sectionName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

    LARGE_INTEGER maxSize;
    maxSize.QuadPart = SHARED_MEMORY_SIZE;

    status = ZwCreateSection(&g_hSection, SECTION_ALL_ACCESS, &objAttr, &maxSize,
        PAGE_READWRITE, SEC_COMMIT, NULL);

    if (!NT_SUCCESS(status)) {
        DbgPrint("[SharedMemory] Failed to create section: 0x%X\n", status);
        return status;
    }

    SIZE_T viewSize = SHARED_MEMORY_SIZE;
    status = ZwMapViewOfSection(g_hSection, NtCurrentProcess(), &g_pSharedMemory, 0, 0,
        NULL, &viewSize, ViewUnmap, 0, PAGE_READWRITE);

    if (!NT_SUCCESS(status)) {
        DbgPrint("[SharedMemory] Failed to map section: 0x%X\n", status);
        ZwClose(g_hSection);
        g_hSection = NULL;
        return status;
    }

    RtlZeroMemory(g_pSharedMemory, SHARED_MEMORY_SIZE);
    DbgPrint("[SharedMemory] Initialized successfully at 0x%p\n", g_pSharedMemory);
    return STATUS_SUCCESS;
}

// Cleanup shared memory
VOID CleanupSharedMemory() {
    if (g_pSharedMemory) {
        ZwUnmapViewOfSection(NtCurrentProcess(), g_pSharedMemory);
        g_pSharedMemory = NULL;
    }

    if (g_hSection) {
        ZwClose(g_hSection);
        g_hSection = NULL;
    }

    DbgPrint("[SharedMemory] Cleanup complete\n");
}

// Write to shared memory
VOID WriteSharedMemory(GameData* data) {
    if (g_pSharedMemory && data) {
        RtlCopyMemory(g_pSharedMemory, data, SHARED_MEMORY_SIZE);
    }
}

// Read from shared memory
VOID ReadSharedMemory(GameData* data) {
    if (g_pSharedMemory && data) {
        RtlCopyMemory(data, g_pSharedMemory, SHARED_MEMORY_SIZE);
    }
}

PVOID GetSharedMemory() {
    return g_pSharedMemory;
}