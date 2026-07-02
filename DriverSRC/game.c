#include <ntddk.h>
#include <wdm.h>
#include "structures.h"
#include "offsets.h"
#include "game.h"

// ============================================================
// GAME DATA READING
// ============================================================

// Initialize game reader
VOID InitializeGameReader() {
    DbgPrint("[GameReader] Initializing...\n");
}

// Cleanup game reader
VOID CleanupGameReader() {
    DbgPrint("[GameReader] Cleaning up...\n");
}

// ============================================================
// MAIN GAME DATA READ
// ============================================================
VOID ReadGameData(GameData* data) {
    if (!data) return;

    RtlZeroMemory(data, sizeof(GameData));

    ReadCameraData(data);
    ReadActorData(data);

    data->timestamp = GetCurrentTimestamp();
    data->isInGame = (data->playerCount > 0);
}

// ============================================================
// CAMERA READING
// ============================================================
VOID ReadCameraData(GameData* data) {
    // Read ViewData base from the pattern address
    uintptr_t viewDataBase = ReadKernelMemory(VIEWDATA_BASE_PTR);
    if (!viewDataBase) {
        DbgPrint("[GameReader] Failed to read ViewData base\n");
        return;
    }

    DbgPrint("[GameReader] ViewData base: 0x%p\n", viewDataBase);

    // Read camera position from ViewData + 0x190
    data->camera.position = ReadVector3(viewDataBase + OFFSET_VIEWDATA_CAMERA_POS);

    // Read view-projection matrix from ViewData + 0x250
    data->camera.viewMatrix = ReadMatrix4x4(viewDataBase + OFFSET_VIEWDATA_VIEW_MATRIX);

    // Read width/height from somewhere else (or hardcode for now)
    data->camera.width = 1920.0f;   // ← Replace with actual resolution
    data->camera.height = 1080.0f;  // ← Replace with actual resolution
    data->camera.fov = 60.0f;       // ← Replace with actual FOV

    DbgPrint("[GameReader] Camera: pos(%.2f, %.2f, %.2f)\n",
        data->camera.position.x, data->camera.position.y, data->camera.position.z);
}

// ============================================================
// ACTOR READING
// ============================================================
VOID ReadActorData(GameData* data) {
    uintptr_t actorBase = ReadKernelMemory(ACTOR_ADDRESS_1);
    if (!actorBase) {
        actorBase = ReadKernelMemory(ACTOR_ADDRESS_2);
        if (!actorBase) {
            DbgPrint("[GameReader] Failed to read actor base\n");
            return;
        }
    }

    uint32_t count = 0;
    Vector3 camPos = data->camera.position;

    for (int i = 0; i < 100 && count < MAX_PLAYERS; i++) {
        uintptr_t actor = ReadKernelMemory(actorBase + (i * 8));
        if (!actor) continue;

        if (!IsValidPlayer(actor)) continue;

        PlayerData* player = &data->players[count];
        player->worldPosition = ReadVector3(actor + OFFSET_ACTOR_POSITION);
        player->actorId = ReadUInt32(actor + OFFSET_ACTOR_ID);
        player->bitfield = ReadUInt64(actor + OFFSET_ACTOR_BITFIELD);
        player->health = ReadUInt32(actor + OFFSET_ACTOR_HEALTH);
        player->aliveFlag = ReadUInt8(actor + OFFSET_ACTOR_ALIVE_FLAG);

        player->isPlayer = ((player->bitfield >> 56) == PLAYER_FLAG_BYTE);
        player->isAlive = (player->health > 0 && player->aliveFlag);
        player->isValid = (player->isPlayer && player->isAlive);

        if (player->isValid) {
            Vector2 screenPos;
            if (WorldToScreen(player->worldPosition, data->camera.position,
                data->camera.viewMatrix, data->camera.projection,
                data->camera.width, data->camera.height, &screenPos)) {
                player->screenPosition = screenPos;
                player->distance = CalculateDistance(camPos, player->worldPosition);
                count++;
            }
        }
    }

    data->playerCount = count;
    DbgPrint("[GameReader] Found %d valid players\n", count);
}

// ============================================================
// PLAYER VALIDATION
// ============================================================
BOOLEAN IsValidPlayer(uintptr_t actor) {
    if (!actor) return FALSE;

    uint64_t bitfield = ReadUInt64(actor + OFFSET_ACTOR_BITFIELD);
    if ((bitfield >> 56) != PLAYER_FLAG_BYTE) return FALSE;

    uint32_t health = ReadUInt32(actor + OFFSET_ACTOR_HEALTH);
    if (health <= 0) return FALSE;

    uint8_t alive = ReadUInt8(actor + OFFSET_ACTOR_ALIVE_FLAG);
    if (!alive) return FALSE;

    return TRUE;
}

// ============================================================
// UTILITY FUNCTIONS
// ============================================================
uint64_t GetCurrentTimestamp() {
    LARGE_INTEGER time;
    KeQuerySystemTime(&time);
    return time.QuadPart;
}