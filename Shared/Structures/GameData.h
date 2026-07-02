#pragma once
#include "CameraData.h"
#include "PlayerData.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    CameraData camera;
    uint32_t playerCount;
    PlayerData players[MAX_PLAYERS];
    uint32_t gameState;
    uint64_t timestamp;
    bool isInGame;
} GameData;

typedef struct {
    uint32_t status;
    uint32_t gamePid;
    uint64_t timestamp;
} DriverStatus;