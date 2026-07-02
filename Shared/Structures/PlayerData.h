#pragma once
#include "Vector.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_PLAYERS 100

typedef struct {
    Vector3 worldPosition;
    Vector2 screenPosition;
    float distance;
    uint32_t health;
    uint32_t actorId;
    uint64_t bitfield;
    uint8_t aliveFlag;
    bool isValid;
    bool isAlive;
    bool isPlayer;
} PlayerData;