#pragma once

#include <cmath>

struct vec {
    float x, y, z;
};

class VecOps {
public:
    vec add(vec v1, vec v2);
    float dot(vec v1, vec v2);
    vec cross(vec v1, vec v2);
    vec scale(vec c, float s);
    float length(vec v);
    vec normalise(vec v);
    float clamp(float v, float min, float max);
};

struct quar {
    float w, i, j, k;
    quar operator+(const quar& other) {
        return { w + w, i + i, j + j, k + k };
    }
};