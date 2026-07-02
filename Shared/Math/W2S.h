#pragma once
#include "../Structures/Vector.h"
#include "../Structures/Matrix.h"

bool WorldToScreen(Vector3 worldPos, Vector3 camPos, Matrix4x4 viewMatrix,
    Matrix4x4 projection, float screenWidth, float screenHeight,
    Vector2* screenPos);

void BuildProjectionMatrix(float fov, float width, float height, Matrix4x4* projection);
float CalculateDistance(Vector3 a, Vector3 b);