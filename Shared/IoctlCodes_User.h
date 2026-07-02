#pragma once
#include <windows.h>

// IOCTL Codes - User Mode Version
// These are the same codes as the kernel version, but defined without ntddk.h

// Define CTL_CODE if not already defined (for user mode)
#ifndef CTL_CODE
#define CTL_CODE(DeviceType, Function, Method, Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#endif

#ifndef METHOD_BUFFERED
#define METHOD_BUFFERED 0
#endif

#ifndef FILE_ANY_ACCESS
#define FILE_ANY_ACCESS 0
#endif

#ifndef FILE_DEVICE_UNKNOWN
#define FILE_DEVICE_UNKNOWN 0x22
#endif

// IOCTL Codes
#define IOCTL_GET_GAME_DATA    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UPDATE_OFFSETS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_STATUS       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_CONFIG       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Shared memory
#define SHARED_MEMORY_NAME L"Global\\RainbowSixESP_SharedMemory"
#define SHARED_MEMORY_SIZE sizeof(GameData)
#define PIPE_NAME L"\\\\.\\pipe\\RainbowSixESP_Pipe"

// Status codes
#define STATUS_DRIVER_LOADED   0x00000001
#define STATUS_DRIVER_UNLOADED 0x00000002
#define STATUS_GAME_FOUND      0x00000010
#define STATUS_GAME_NOT_FOUND  0x00000011