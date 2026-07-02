#include "pch.h"
#include "client.h"
#include "structures.h"
#include "../Shared/Communication/IoctlCodes_User.h"

GameClient::GameClient() : m_driverHandle(INVALID_HANDLE_VALUE),
m_sharedMemoryHandle(INVALID_HANDLE_VALUE),
m_sharedMemory(NULL), m_connected(false) {}

GameClient::~GameClient() { Disconnect(); }

bool GameClient::Connect() {
    if (m_connected) return true;
    if (ConnectSharedMemory()) { m_connected = true; return true; }
    if (ConnectIoctl()) { m_connected = true; return true; }
    return false;
}

void GameClient::Disconnect() {
    if (m_sharedMemory) { UnmapViewOfFile(m_sharedMemory); m_sharedMemory = NULL; }
    if (m_sharedMemoryHandle != INVALID_HANDLE_VALUE) { CloseHandle(m_sharedMemoryHandle); m_sharedMemoryHandle = INVALID_HANDLE_VALUE; }
    if (m_driverHandle != INVALID_HANDLE_VALUE) { CloseHandle(m_driverHandle); m_driverHandle = INVALID_HANDLE_VALUE; }
    m_connected = false;
}

bool GameClient::GetGameData(GameData* data) {
    if (!m_connected || !data) return false;

    if (m_sharedMemory) {
        // ← ADD THIS: Check if shared memory is valid
        __try {
            memcpy(data, m_sharedMemory, sizeof(GameData));
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }

        // ← ADD THIS: Validate the data
        if (data->playerCount > 100) {
            memset(data, 0, sizeof(GameData));
            return false;
        }

        return true;
    }

    if (m_driverHandle != INVALID_HANDLE_VALUE) {
        DWORD bytesReturned;
        return DeviceIoControl(m_driverHandle, IOCTL_GET_GAME_DATA,
            NULL, 0, data, sizeof(GameData),
            &bytesReturned, NULL);
    }
    return false;
}

bool GameClient::ConnectSharedMemory() {
    m_sharedMemoryHandle = OpenFileMappingA(FILE_MAP_READ, FALSE, "Global\\RainbowSixESP_SharedMemory");
    if (m_sharedMemoryHandle == INVALID_HANDLE_VALUE) return false;

    m_sharedMemory = (GameData*)MapViewOfFile(m_sharedMemoryHandle, FILE_MAP_READ, 0, 0, 0);
    if (!m_sharedMemory) {
        CloseHandle(m_sharedMemoryHandle);
        m_sharedMemoryHandle = INVALID_HANDLE_VALUE;
        return false;
    }
    return true;
}

bool GameClient::ConnectIoctl() {
    m_driverHandle = CreateFileA("\\\\.\\RainbowSixESP", GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);
    return m_driverHandle != INVALID_HANDLE_VALUE;
}

