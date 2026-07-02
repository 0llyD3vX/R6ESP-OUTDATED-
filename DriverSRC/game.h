#pragma once
#include "structures.h"
#include "offsets.h"

// Game functions
VOID InitializeGameReader();
VOID CleanupGameReader();
VOID ReadGameData(GameData* data);
VOID ReadCameraData(GameData* data);
VOID ReadActorData(GameData* data);
BOOLEAN IsValidPlayer(uintptr_t actor);
uint64_t GetCurrentTimestamp();

// Memory functions (from memory.c)
uint64_t ReadKernelMemory(uint64_t address);
uint64_t ReadProcessMemory(PEPROCESS process, uint64_t address);
Vector3 ReadVector3(uint64_t address);
float ReadFloat(uint64_t address);
uint32_t ReadUInt32(uint64_t address);
uint64_t ReadUInt64(uint64_t address);
uint8_t ReadUInt8(uint64_t address);
Matrix4x4 ReadMatrix4x4(uint64_t address);
BOOLEAN IsValidPointer(uint64_t address);
BOOLEAN IsAddressReadable(uint64_t address);

// W2S functions (from w2s.c)
BOOLEAN WorldToScreen(Vector3 worldPos, Vector3 camPos, Matrix4x4 viewMatrix,
    Matrix4x4 projection, float screenWidth, float screenHeight,
    Vector2* screenPos);
void BuildProjectionMatrix(CameraData* camera);
float CalculateDistance(Vector3 a, Vector3 b);
float VectorLength(Vector3 v);
Vector3 VectorSubtract(Vector3 a, Vector3 b);
Vector3 VectorAdd(Vector3 a, Vector3 b);
Vector3 VectorMultiply(Vector3 v, float scalar);

// Shared memory functions (from shared_memory.c)
NTSTATUS InitializeSharedMemory();
VOID CleanupSharedMemory();
VOID WriteSharedMemory(GameData* data);
VOID ReadSharedMemory(GameData* data);
PVOID GetSharedMemory();