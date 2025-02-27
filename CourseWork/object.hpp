#pragma once

#include <vector>

#include "vec.hpp"

struct ObjectData {
    int type, material;
    vec r;
    float l1, mass;
    vec vel, impulse;
    bool down = true, moving = false, floor = false;
    float lastBounceSpeed;
};

class Object {
protected:
    ObjectData data;
    VecOps vOps;

    float SDF();
public:
    Object(ObjectData data);

    void update(float f, float dt);

    void updateObject(float g, float r, float dt);

    bool checkCollision(Object& other);

    void resolveCollision(Object& other);

    ObjectData *getData();
};

class Sphere : public Object {
protected:
    float SDF(vec p, vec c, float r);
public:
    Sphere(ObjectData data) : Object(data) {}
};