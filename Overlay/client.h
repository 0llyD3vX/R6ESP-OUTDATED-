#pragma once
#include "structures.h"

class GameClient {
private:
    HANDLE m_driverHandle;
    HANDLE m_sharedMemoryHandle;
    GameData* m_sharedMemory;
    bool m_connected;

    bool ConnectSharedMemory();
    bool ConnectIoctl();

public:
    GameClient();
    ~GameClient();

    bool Connect();
    void Disconnect();
    bool GetGameData(GameData* data);
    bool IsConnected() { return m_connected; }
};