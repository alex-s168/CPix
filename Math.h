#ifndef UNIPIX_MATH_H
#define UNIPIX_MATH_H

float Lerpf(float a, float b, float t) {
    if (t > 1) {
        t = 1;
    }
    if (t < 0) {
        t = 0;
    }
    return (1.0f - t) * a + b * t;
}

int Lerpi(int a, int b, float t) {
    if (t > 1) {
        t = 1;
    }
    if (t < 0) {
        t = 0;
    }
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

#endif //UNIPIX_MATH_H