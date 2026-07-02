#pragma once
#include "../Structures/Vector.h"

float VectorLength(Vector3 v);
Vector3 VectorSubtract(Vector3 a, Vector3 b);
Vector3 VectorAdd(Vector3 a, Vector3 b);
Vector3 VectorMultiply(Vector3 v, float scalar);
float VectorDot(Vector3 a, Vector3 b);
Vector3 VectorCross(Vector3 a, Vector3 b);