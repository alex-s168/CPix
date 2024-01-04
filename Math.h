#ifndef UNIPIX_MATH_H
#define UNIPIX_MATH_H

#include <math.h>

float Lerpf(float a, float b, float t) {
    return (1.0f - t) * a + b * t;
}

int Lerpi(int a, int b, float t) {
    return (int) Lerpf((float) a, (float) b, t);
}

float InvLerpf(float a, float b, float v) {
    return (v - a) / (b - a);
}

float InvLerpi(int a, int b, int v) {
    return InvLerpf((float) a, (float) b, (float) v);
}

float Remapf(float iMin, float iMax, float oMin, float oMax, float v) {
    float t = InvLerpf(iMin, iMax, v);
    return Lerpf(oMin, oMax, t);
}

int Remapi(int iMin, int iMax, int oMin, int oMax, int v) {
    float t = InvLerpi(iMin, iMax, v);
    return Lerpi(oMin, oMax, t);
}

#define PI M_PI

float pi180 = (float) PI / 180.0f;

float Radians(float degrees) {
    return degrees * pi180;
}

#ifndef MICOWEB_MATRIX_H
#include "Matrix.h"
#endif

Matrix *RotMat2f(Matrix *destIn, float radians) {
    Matrix *dest;
    if (destIn == NULL) {
        dest = malloc(sizeof(Matrix));
        if (dest == NULL) {
            return dest;
        }
        dest->data = NULL;
    }
    else {
        dest = destIn;
    }
    if (dest->data == NULL) {
        MatrixInit(dest, 2, 2, sizeof(float));
    }

    float *data = dest->data;

    data[0] = cosf(radians);
    data[1] = -sinf(radians);

    data[2] = sinf(radians);
    data[3] = cosf(radians);

    return dest;
}

typedef struct {
    float x;
    float y;
} Vec2f;

Vec2f Rot2f(const Vec2f in, const Matrix rotMat2f) {
    float *rm = rotMat2f.data;
    return (Vec2f) {
        .x = rm[0] * in.x + rm[1] * in.y,
        .y = rm[2] * in.x + rm[3] * in.y
    };
}

typedef struct {
    Vec2f a;
    Vec2f b;
    Vec2f c;
    Vec2f d;
} Poly4f;

Poly4f Poly4fRot2f(const Poly4f in, const Matrix rotMat2f) {
    return (Poly4f) {
        Rot2f(in.a, rotMat2f),
        Rot2f(in.b, rotMat2f),
        Rot2f(in.c, rotMat2f),
        Rot2f(in.d, rotMat2f),
    };
}

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

Vec2f MaxVec2f(Vec2f a, Vec2f b) {
    return (Vec2f) { MAX(a.x, b.x), MAX(a.y, b.y) };
}

Vec2f Poly4fBounds(Poly4f poly) {
    return MaxVec2f(MaxVec2f(MaxVec2f(poly.a, poly.b), poly.c), poly.d);
}

#endif //UNIPIX_MATH_H