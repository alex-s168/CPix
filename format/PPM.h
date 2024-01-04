#ifndef UNIPIX_PPM_H
#define UNIPIX_PPM_H

#include "stdio.h"
#include "stdbool.h"

#include "../Matrix.h"

enum PPMErr {
    PPMErr_OK,       // No error
    PPMErr_InvFile,  // Invalid File
    PPMErr_ModeNS,   // Mode not supported
    PPMErr_NoMem,    // Memory allocation error
};

static char *Until(FILE *file, char value) {
    size_t bufSize = 50;
    char *buf = malloc(bufSize);
    if (buf == NULL) {
        return NULL;
    }
    size_t next = 0;

    int c;
    while ((c = getc(file)) != value && c != EOF) {
        buf[next++] = (char) c;
        if (next == bufSize) {
            bufSize += 50;
            buf = realloc(buf, bufSize);
            if (buf == NULL) {
                return NULL;
            }
        }
    }

    buf[next] = '\0';

    return buf;
}

/**
 * Returns a matrix of 3 byte arrays = image (rgb)
 */
Matrix PPM(enum PPMErr *err, FILE *file, bool autoConvertTo255, bool to4) {
    if (getc(file) != 'P') {
        *err = PPMErr_InvFile;
        return EmptyMatrix();
    }
    if (getc(file) != '6') {
        *err = PPMErr_ModeNS;
        return EmptyMatrix();
    }
    if (getc(file) != '\n') {
        *err = PPMErr_InvFile;
        return EmptyMatrix();
    }

    char *buf = Until(file, ' ');
    size_t w = atoi(buf);
    free(buf);

    buf = Until(file, '\n');
    size_t h = atoi(buf);
    free(buf);

    buf = Until(file, '\n');
    size_t maxValPerChannel = atoi(buf);
    free(buf);

    Matrix mat;
    if (to4) {
        size_t w3 = w * 3;
        size_t fixed = FixWidth(w);
        size_t w3m = fixed * 3;
        MatrixInit(&mat, fixed, h, 3);

        int c;
        for (size_t y = 0; y < h; y ++) {
            char *line = MatrixOffset(mat, 0, y);
            for (size_t x = 0; x < w3; x ++) {
                c = getc(file);
                if (c == EOF) {
                    break;
                }
                *(line + x) = (char) c;
            }
            for (size_t i = w3; i < w3m; i ++) {
                *(line + i) = 0;
            }
            if (c == EOF) {
                break;
            }
        }
    }
    else {
        MatrixInit(&mat, w, h, 3);

        int c;
        size_t i = 0;
        size_t max = w * h * 3;
        while (i < max && (c = getc(file)) != EOF) {
            *((char *) mat.data + i) = (char) c;
            i++;
        }
    }

    if (maxValPerChannel != 255 && autoConvertTo255) {
        MatrixiRemap(mat, 0, (int) maxValPerChannel, 0, 255);
    }

    *err = PPMErr_OK;
    return mat;
}

#endif //UNIPIX_PPM_H
