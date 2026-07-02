#pragma once
#include "../Structures/Matrix.h"

void MatrixMultiply(Matrix4x4 a, Matrix4x4 b, Matrix4x4* result);
void MatrixIdentity(Matrix4x4* matrix);
void MatrixTranspose(Matrix4x4* matrix);