#include <math.h>
#include "W2S.h"
#include "VectorMath.h"
#include "MatrixMath.h"

bool WorldToScreen(Vector3 worldPos, Matrix4x4 viewProjMatrix,
    float screenWidth, float screenHeight, Vector2* screenPos) {
    // Transform world to clip space using view-projection matrix
    Vector4 clipPos;
    clipPos.x = worldPos.x * viewProjMatrix.m[0][0] + worldPos.y * viewProjMatrix.m[1][0] +
        worldPos.z * viewProjMatrix.m[2][0] + viewProjMatrix.m[3][0];
    clipPos.y = worldPos.x * viewProjMatrix.m[0][1] + worldPos.y * viewProjMatrix.m[1][1] +
        worldPos.z * viewProjMatrix.m[2][1] + viewProjMatrix.m[3][1];
    clipPos.z = worldPos.x * viewProjMatrix.m[0][2] + worldPos.y * viewProjMatrix.m[1][2] +
        worldPos.z * viewProjMatrix.m[2][2] + viewProjMatrix.m[3][2];
    clipPos.w = worldPos.x * viewProjMatrix.m[0][3] + worldPos.y * viewProjMatrix.m[1][3] +
        worldPos.z * viewProjMatrix.m[2][3] + viewProjMatrix.m[3][3];

    if (clipPos.w < 0.001f) return false;

    // Perspective divide
    Vector3 ndc;
    ndc.x = clipPos.x / clipPos.w;
    ndc.y = clipPos.y / clipPos.w;
    ndc.z = clipPos.z / clipPos.w;

    // Check if on screen
    if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f) {
        return false;
    }

    // Convert to screen coordinates
    screenPos->x = (ndc.x * 0.5f + 0.5f) * screenWidth;
    screenPos->y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;

    return true;
}

void BuildProjectionMatrix(float fov, float width, float height, Matrix4x4* projection) {
    float fovRad = fov * 3.14159265f / 180.0f;
    float aspect = width / height;
    float f = 1.0f / tanf(fovRad / 2.0f);
    float near = 0.1f;
    float far = 10000.0f;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            projection->m[i][j] = 0.0f;
        }
    }

    projection->m[0][0] = f / aspect;
    projection->m[1][1] = f;
    projection->m[2][2] = (far + near) / (near - far);
    projection->m[2][3] = (2.0f * far * near) / (near - far);
    projection->m[3][2] = -1.0f;
}

float CalculateDistance(Vector3 a, Vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}