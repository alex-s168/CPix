#ifndef MICOWEB_MATRIX_H
#define MICOWEB_MATRIX_H

#include <stddef.h>
#include <stdlib.h>
#include <strings.h>

typedef struct {
    size_t width;
    size_t height;
    size_t elementSize;
    void *data;
} Matrix;

static Matrix empty = (Matrix) { .width = 0, .height = 0, .elementSize = 0 };

Matrix EmptyMatrix() {
    return empty;
}

void MatrixInit(Matrix *mat, size_t width, size_t height, size_t elementSize) {
    void *data = malloc(elementSize * (width * height + 1));
    memset(data + elementSize * width * height, 0, elementSize);
    *mat = (Matrix) {.width = width, .height = height, .elementSize = elementSize, .data = data};
}

static void RotateInPlace(void *data, size_t elementSize, size_t length) {
    for (size_t e = 0; e < length; e ++) {
        void *ePos = data + e * elementSize;
        void *ePosNew = data + (length - e - 1) * elementSize;
        memcpy(ePosNew, ePos, elementSize);
    }
}

void MatrixRotateBlocks(Matrix mat, size_t blockLength) {
    size_t blocks = mat.width * mat.height / blockLength;
    size_t blockSize = blockLength * mat.elementSize;
    void *bp = mat.data;
    for (size_t b = 0; b < blocks; b ++) {
        RotateInPlace(bp, mat.elementSize, blockLength);
        bp += blockSize;
    }
}

#ifndef UNIPIX_MATH_H
#include "Math.h"
#endif

void MatrixfRemap(Matrix mat, float iMin, float iMax, float oMin, float oMax) {
    float *data = (float *) mat.data;
    float *dataEnd = data + (mat.width * mat.height);
    while (data < dataEnd) {
        *data = Remapf(iMin, iMax, oMin, oMax, *data);
        ++ data;
    }
}

void MatrixiRemap(Matrix mat, int iMin, int iMax, int oMin, int oMax) {
    int *data = (int *) mat.data;
    int *dataEnd = data + (mat.width * mat.height);
    while (data < dataEnd) {
        *data = Remapi(iMin, iMax, oMin, oMax, *data);
        ++ data;
    }
}

void MatrixFill(Matrix mat, const void *element) {
    void *data = mat.data;
    size_t es = mat.elementSize;
    void *dataEnd = mat.data + (mat.width * mat.height) * es;
    while (data < dataEnd) {
        memcpy(data, element, es);
        data += es;
    }
}

void *MatrixOffset(Matrix mat, size_t x, size_t y) {
    if (x > mat.width) {
        x = mat.width; // the one extra 0 element
    }
    if (y > mat.height) {
        y = mat.height; // the one extra 0 element
    }
    return ((char *) mat.data) + (y * mat.width + x) * mat.elementSize;
}

void MatrixClear(Matrix *mat) {
    free(mat->data);
    mat->width = 0;
    mat->height = 0;
}

#endif //MICOWEB_MATRIX_H
