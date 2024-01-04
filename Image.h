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

Matrix *Upscale(const Matrix mat, Matrix *dest, float sx, float sy) {
    size_t newWidth = FixWidth((size_t) (((float) mat.width) * sx));
    size_t newHeight = (size_t) (((float) mat.height) * sy);

    Matrix *new;
    if (dest == NULL) {
        new = malloc(sizeof(Matrix));
        MatrixInit(new, newWidth, newHeight, 3);
    }
    else {
        new = dest;
        if (newWidth > dest->width) {
            newWidth = dest->width;
        }
        if (newHeight > dest->height) {
            newHeight = dest->height;
        }
    }

    for (size_t y = 0; y < newHeight; y ++) {
        float oldY = (float) y / sy;
        float oldYT = oldY - (float) ((int) oldY);
        size_t oldYB = (size_t) (oldY - oldYT);
        for (size_t x = 0; x < newWidth; x ++) {
            float oldX = (float) x / sx;
            float oldXT = oldX - (float) (int) oldX;
            size_t oldXB = (size_t) (oldX - oldXT);
            char *x1 = MatrixOffset(mat, oldXB, oldYB);
            char *x2 = MatrixOffset(mat, oldXB + 1, oldYB);
            char *x3 = MatrixOffset(mat, oldXB, oldYB + 1);
            char *x4 = MatrixOffset(mat, oldXB + 1, oldYB + 1);
            char left[3] = {
                    (char) Lerpi(x1[0], x3[0], oldYT),
                    (char) Lerpi(x1[1], x3[1], oldYT),
                    (char) Lerpi(x1[2], x3[2], oldYT)
            };
            char right[3] = {
                    (char) Lerpi(x2[0], x4[0], oldYT),
                    (char) Lerpi(x2[1], x4[1], oldYT),
                    (char) Lerpi(x2[2], x4[2], oldYT)
            };
            char *x0 = MatrixOffset(*new, x, y);
            x0[0] = (char) Lerpi(left[0], right[0], oldXT);
            x0[1] = (char) Lerpi(left[1], right[1], oldXT);
            x0[2] = (char) Lerpi(left[2], right[2], oldXT);
        }
    }

    return new;
}

Matrix *UpscaleNearest(const Matrix mat, Matrix *dest, float sx, float sy) {
    size_t newWidth = FixWidth((size_t) (((float) mat.width) * sx));
    size_t newHeight = (size_t) (((float) mat.height) * sy);
    Matrix *new;
    if (dest == NULL) {
        new = malloc(sizeof(Matrix));
        MatrixInit(new, newWidth, newHeight, 3);
    }
    else {
        new = dest;
        if (newWidth > dest->width) {
            newWidth = dest->width;
        }
        if (newHeight > dest->height) {
            newHeight = dest->height;
        }
    }

    for (size_t y = 0; y < newHeight; y ++) {
        float oldY = (float) y / sy;
        for (size_t x = 0; x < newWidth; x ++) {
            float oldX = (float) x / sx;
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
