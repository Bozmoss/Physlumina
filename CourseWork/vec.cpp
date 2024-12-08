#include "vec.hpp"

vec VecOps::add(vec v1, vec v2) {
    return vec{
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    };
}

float VecOps::dot(vec v1, vec v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec VecOps::cross(vec v1, vec v2) {
    return vec{
        v1.y * v2.z - v1.z * v2.y,
        v1.y * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

vec VecOps::scale(vec v, float s) {
    return vec{
        v.x * s,
        v.y * s,
        v.z * s
    };
}

float VecOps::length(vec v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec VecOps::normalise(vec v) {
    return scale(v, 1.0f/length(v));
}

float VecOps::clamp(float v, float min, float max) {
    return (v < min) ? min : (v > max) ? max : v;
}