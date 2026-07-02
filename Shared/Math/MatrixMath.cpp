#include "MatrixMath.h"

void MatrixMultiply(Matrix4x4 a, Matrix4x4 b, Matrix4x4* result) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result->m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void MatrixIdentity(Matrix4x4* matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix->m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

void MatrixTranspose(Matrix4x4* matrix) {
    Matrix4x4 temp = *matrix;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix->m[i][j] = temp.m[j][i];
        }
    }
}