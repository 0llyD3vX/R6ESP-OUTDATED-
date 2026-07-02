#pragma once
#include <ntddk.h>
#include <wdm.h>
#include "offsets.h"
#include "structures.h"

// ============================================================
// MEMORY READING FUNCTIONS
// ============================================================

// Read kernel memory (physical memory)
uint64_t ReadKernelMemory(uint64_t address) {
    if (!address) return 0;

    __try {
        return *(uint64_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

// Read process memory (virtual) - Updated
uint64_t ReadProcessMemory(PEPROCESS process, uint64_t address) {
    if (!address || !process) return 0;

    uint64_t result = 0;
    SIZE_T bytesRead = 0;
    MM_COPY_ADDRESS copyAddress = { 0 };
    copyAddress.VirtualAddress = (PVOID)address;

    __try {
        MmCopyMemory((PVOID)&result, copyAddress, sizeof(uint64_t), MM_COPY_MEMORY_VIRTUAL, &bytesRead);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }

    return (bytesRead == sizeof(uint64_t)) ? result : 0;
}

// Read vector3
Vector3 ReadVector3(uint64_t address) {
    Vector3 result = { 0 };
    if (!address) return result;

    __try {
        result.x = *(float*)(address);
        result.y = *(float*)(address + 4);
        result.z = *(float*)(address + 8);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // Return zeros
    }

    return result;
}

// Read float
float ReadFloat(uint64_t address) {
    if (!address) return 0.0f;

    __try {
        return *(float*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0.0f;
    }
}

// Read uint32
uint32_t ReadUInt32(uint64_t address) {
    if (!address) return 0;

    __try {
        return *(uint32_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

// Read uint64
uint64_t ReadUInt64(uint64_t address) {
    if (!address) return 0;

    __try {
        return *(uint64_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

// Read uint8
uint8_t ReadUInt8(uint64_t address) {
    if (!address) return 0;

    __try {
        return *(uint8_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

// Read matrix4x4
Matrix4x4 ReadMatrix4x4(uint64_t address) {
    Matrix4x4 result = { 0 };
    if (!address) return result;

    __try {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = *(float*)(address + (i * 16 + j * 4));
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // Return identity
        result.m[0][0] = 1.0f;
        result.m[1][1] = 1.0f;
        result.m[2][2] = 1.0f;
        result.m[3][3] = 1.0f;
    }

    return result;
}

// ============================================================
// MEMORY VALIDATION
// ============================================================
BOOLEAN IsValidPointer(uint64_t address) {
    if (!address) return FALSE;
    if (address < 0x10000) return FALSE;
    if (address > 0x7FFFFFFFFFFF) return FALSE;
    return TRUE;
}

BOOLEAN IsAddressReadable(uint64_t address) {
    __try {
        volatile uint8_t test = *(uint8_t*)address;
        return TRUE;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return FALSE;
    }
}
