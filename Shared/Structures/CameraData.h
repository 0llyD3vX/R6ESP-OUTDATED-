#pragma once
#include "Vector.h"
#include "Matrix.h"

typedef struct {
    Vector3 position;
    float width;
    float height;
    float fov;
    Matrix4x4 viewMatrix;
    Matrix4x4 projection;
} CameraData;