#pragma once
#include <ntddk.h>
#include <wdm.h>
#include "structures.h"
#include <math.h>
// ============================================================
// WORLD TO SCREEN
// ============================================================
BOOLEAN WorldToScreen(Vector3 worldPos, Vector3 camPos, Matrix4x4 viewMatrix,
    Matrix4x4 projection, float screenWidth, float screenHeight,
    Vector2* screenPos) {

    // Transform world to camera space
    Vector4 clipPos;
    clipPos.x = worldPos.x * viewMatrix.m[0][0] + worldPos.y * viewMatrix.m[1][0] +
        worldPos.z * viewMatrix.m[2][0] + viewMatrix.m[3][0];
    clipPos.y = worldPos.x * viewMatrix.m[0][1] + worldPos.y * viewMatrix.m[1][1] +
        worldPos.z * viewMatrix.m[2][1] + viewMatrix.m[3][1];
    clipPos.z = worldPos.x * viewMatrix.m[0][2] + worldPos.y * viewMatrix.m[1][2] +
        worldPos.z * viewMatrix.m[2][2] + viewMatrix.m[3][2];
    clipPos.w = worldPos.x * viewMatrix.m[0][3] + worldPos.y * viewMatrix.m[1][3] +
        worldPos.z * viewMatrix.m[2][3] + viewMatrix.m[3][3];

    // Apply projection
    Vector4 projected;
    projected.x = clipPos.x * projection.m[0][0] + clipPos.y * projection.m[1][0] +
        clipPos.z * projection.m[2][0] + clipPos.w * projection.m[3][0];
    projected.y = clipPos.x * projection.m[0][1] + clipPos.y * projection.m[1][1] +
        clipPos.z * projection.m[2][1] + clipPos.w * projection.m[3][1];
    projected.z = clipPos.x * projection.m[0][2] + clipPos.y * projection.m[1][2] +
        clipPos.z * projection.m[2][2] + clipPos.w * projection.m[3][2];
    projected.w = clipPos.x * projection.m[0][3] + clipPos.y * projection.m[1][3] +
        clipPos.z * projection.m[2][3] + clipPos.w * projection.m[3][3];

    // Check if behind camera
    if (projected.w < 0.001f) return FALSE;

    // Perspective divide
    Vector3 ndc;
    ndc.x = projected.x / projected.w;
    ndc.y = projected.y / projected.w;
    ndc.z = projected.z / projected.w;

    // Check if on screen (NDC bounds)
    if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f) {
        return FALSE;
    }

    // Convert to screen coordinates
    screenPos->x = (ndc.x * 0.5f + 0.5f) * screenWidth;
    screenPos->y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;

    return TRUE;
}

// ============================================================
// PROJECTION MATRIX BUILDING
// ============================================================
void BuildProjectionMatrix(CameraData* camera) {
    float fovRad = camera->fov * 3.14159265f / 180.0f;
    float aspect = camera->width / camera->height;
    float f = 1.0f / tan(fovRad / 2.0f);
    float near = 0.1f;
    float far = 10000.0f;

    Matrix4x4* proj = &camera->projection;

    // Zero out
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            proj->m[i][j] = 0.0f;
        }
    }

    proj->m[0][0] = f / aspect;
    proj->m[1][1] = f;
    proj->m[2][2] = (far + near) / (near - far);
    proj->m[2][3] = (2.0f * far * near) / (near - far);
    proj->m[3][2] = -1.0f;
}

// ============================================================
// DISTANCE CALCULATIONS
// ============================================================
float CalculateDistance(Vector3 a, Vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

float VectorLength(Vector3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 VectorSubtract(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

Vector3 VectorAdd(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vector3 VectorMultiply(Vector3 v, float scalar) {
    Vector3 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}
