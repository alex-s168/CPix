#ifndef UNIPIX_IMAGE_H
#define UNIPIX_IMAGE_H

#include "Matrix.h"
#include "Math.h"

size_t FixWidth(size_t value) {
    size_t res = value * 3;

    if (res % 4 == 0) {
        return res;
    }

    size_t rem = res % 4;
    size_t toAdd = 4 - rem;
    size_t mod = res + toAdd;

    return mod;
}

Matrix *Upscale(Matrix mat, float sx, float sy) {
    Matrix *new = malloc(sizeof(Matrix));
    size_t newWidth = FixWidth((size_t) (((float) mat.width) * sx));
    size_t newHeight = (size_t) (((float) mat.height) * sy);
    MatrixInit(new, newWidth, newHeight, 3);

    for (size_t y = 0; y < newHeight; y ++) {
        float oldY = (float) y / sy;
        float oldYT = oldY - (float) ((int) oldY);
        size_t oldYB = (size_t) (oldY - oldYT);
        for (size_t x = 0; x < newWidth; x ++) {
            float oldX = (float) x / sy;
            float oldXT = oldX - (float) (int) oldX;
            size_t oldXB = (size_t) (oldX - oldXT);
            char *x1 = MatrixOffset(mat, oldXB, oldYB);
            char *x2 = MatrixOffset(mat, oldXB + 1, oldYB);
            char *x3 = MatrixOffset(*new, x, y);
            x3[0] = (char) Lerpi(x1[0], x2[0], oldXT);
            x3[1] = (char) Lerpi(x1[1], x2[1], oldXT);
            x3[2] = (char) Lerpi(x1[2], x2[2], oldXT);
        }
    }

    return new;
}

Matrix *UpscaleSimple(Matrix mat, float sx, float sy) {
    Matrix *new = malloc(sizeof(Matrix));
    size_t newWidth = FixWidth((size_t) (((float) mat.width) * sx));
    size_t newHeight = (size_t) (((float) mat.height) * sy);
    MatrixInit(new, newWidth, newHeight, 3);

    for (size_t y = 0; y < newHeight; y ++) {
        float oldY = (float) y / sy;
        for (size_t x = 0; x < newWidth; x ++) {
            float oldX = (float) x / sy;
            char *x1 = MatrixOffset(mat,(size_t) oldX, (size_t) oldY);
            char *x3 = MatrixOffset(*new, x, y);
            x3[0] = x1[0];
            x3[1] = x1[1];
            x3[2] = x1[2];
        }
    }

    return new;
}

#endif //UNIPIX_IMAGE_H
