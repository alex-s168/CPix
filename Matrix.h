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

void MatrixRotateBlocks(Matrix mat, size_t blockLength) {
    size_t bs = blockLength * mat.elementSize;
    void *buff = malloc(bs);
    for (size_t i = 0; i < (mat.width * mat.width) / blockLength; i ++) {
        char *bb = (char *) mat.data + i * bs;
        for (size_t e = 0; e < blockLength; e ++) {
            memcpy(buff + (blockLength - e - 1) * mat.elementSize, bb + e * mat.elementSize, mat.elementSize);
        }
        memcpy(bb, buff, bs);
    }
    free(buff);
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
