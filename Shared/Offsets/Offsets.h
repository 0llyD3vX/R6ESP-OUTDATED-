#pragma once

// ============================================================
// R6S Offsets - v4.0
// Date: 2026-06-23
// ============================================================

// === VIEWDATA (Found via pattern scan) ===
#define VIEWDATA_BASE_PTR 0x1AD49F3F114        // ← Pattern address
#define OFFSET_VIEWDATA_CAMERA_POS  0x190      // Vector3
#define OFFSET_VIEWDATA_VIEW_MATRIX 0x250      // 4x4 matrix

// === ACTOR SYSTEM ===
#define ACTOR_ADDRESS_1 0x7FF6128AF694         // ← From your scan
#define ACTOR_ADDRESS_2 0x7FF61608CE5F         // ← From your scan
#define OFFSET_ACTOR_POSITION 0x50
#define OFFSET_ACTOR_ID 0xAC
#define OFFSET_ACTOR_BITFIELD 0xB0
#define OFFSET_ACTOR_HEALTH 0x280
#define OFFSET_ACTOR_ALIVE_FLAG 0x1D1

// === VALIDATION ===
#define PLAYER_FLAG_BYTE 0x24

// === ENGINE ===
#define ENGINE_SPEED 0x7FF61427413B