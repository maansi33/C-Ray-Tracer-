#include <stdio.h>
#include <math.h>
#include "vector.h"

Vec3 add(Vec3 v1, Vec3 v2) {
    Vec3 result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    return result;
}

Vec3 subtract(Vec3 v1, Vec3 v2) {
    Vec3 result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return result;
}

Vec3 normalize(Vec3 v) {
    float len = length(v);
    if (len > 0) {
        return scalarMultiply(1.0f / len, v);
    }
    return v;  
}

float dot(Vec3 v1, Vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


float distance(Vec3 v1, Vec3 v2) {
    return sqrtf((float)((v2.x - v1.x) * (v2.x - v1.x)) +
                 (float)((v2.y - v1.y) * (v2.y - v1.y)) +
                 (float)((v2.z - v1.z) * (v2.z - v1.z)));
}

float distance2(Vec3 v1, Vec3 v2) {
    return length2(v1) - length2(v2); 
}



float length(Vec3 v) {
    return sqrtf((float)(v.x * v.x + v.y * v.y + v.z * v.z));
}

float length2(Vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vec3 scalarMultiply(float s, Vec3 v) {
    Vec3 result = {s * v.x, s * v.y, s * v.z};
    return result;
}

Vec3 scalarDivide(Vec3 v, float d) {
    if (d != 0) {
        Vec3 result = {v.x / (float)d, v.y / (float)d, v.z / (float)d};
        return result;
    }
    return (Vec3){0.0f, 0.0f, 0.0f}; 
}

Vec3 zeroVector() {
    return (Vec3){0.0f, 0.0f, 0.0f};
}

