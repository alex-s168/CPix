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
        new->data = NULL;
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
    if (new->data == NULL) {
        MatrixInit(new, newWidth, newHeight, 3);
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
        new->data = NULL;
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
    if (new->data == NULL) {
        MatrixInit(new, newWidth, newHeight, 3);
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

Matrix *Rotatef(const Matrix mat, Matrix *destIn, Matrix rot2f) {
    Poly4f oldSize = (Poly4f) {
            (Vec2f) { 0, 0 },
            (Vec2f) { (float) mat.width, 0 },
            (Vec2f) { 0, (float) mat.height },
            (Vec2f) { (float) mat.width, (float) mat.height}
    };
    Poly4f transformed = Poly4fRot2f(oldSize, rot2f);
    Vec2f bounds = Poly4fBounds(transformed);
    size_t newW = (size_t) bounds.x;
    size_t newH = (size_t) bounds.y;

    Matrix *dest;
    if (destIn == NULL) {
        dest = malloc(sizeof(Matrix));
        dest->data = NULL;
    }
    else {
        dest = destIn;
    }
    if (dest->data == NULL) {
        MatrixInit(dest, newW, newH, 3);
    }

    for (size_t i = 0; i < mat.width * mat.height; i ++) {
        char *color = ((char *) mat.data) + i;
        size_t x = i % mat.width;
        size_t y = i / mat.height;
        Vec2f r = Rot2f((Vec2f) { (float) x, (float) y }, rot2f);
        char *other = MatrixOffset(*dest, (size_t) r.x, (size_t) r.y);
        other[0] = color[0];
        other[1] = color[1];
        other[2] = color[2];
    }

    return dest;
}

#endif //UNIPIX_IMAGE_H
