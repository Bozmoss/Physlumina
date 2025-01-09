#include "object.hpp"

#include <iostream>

Object::Object(ObjectData data) :
    data{ data }{}

float Object::SDF() { return 0; }

void Object::update(float f) {
    if (data.r.y - data.l1 <= -1.5) {
        data.vel.x *= f;
        data.vel.z *= f;
    }
    data.r = vOps.add(data.r, data.vel);
}

void Object::updateObject(float g, float r) {
    if (data.r.y - data.l1 <= -1.5) {
        data.r.y = data.l1 - 1.5;
        if (vOps.length(data.vel) < 0.001f) {
            data.vel = { 0.0, 0.0, 0.0 };
        }
        else {
            vec colNorm = vOps.normalise(vec{ 0.0, data.r.y, 0.0 });
            vec relVel = vOps.scale(data.vel, -1);
            float velNorm = vOps.dot(relVel, colNorm);
            float j = -(1 + r) * velNorm * data.mass;
            vec impulse = vOps.scale(colNorm, j);
            data.vel = vOps.add(data.vel, vOps.scale(impulse, -1 / data.mass));
            data.vel = vOps.add(data.vel, { 0.0, -0.001, 0.0 });
        }
    }
    else {
        data.vel.y -= g;
    }
}

bool Object::checkCollision(Object& other) {
    if (this == &other) return false;
    float dist = vOps.length(vOps.add(data.r, vOps.scale(other.getData()->r, -1)));
    return dist < (data.l1 + other.getData()->l1);
}

void Object::resolveCollision(Object& other) {
    vec col = vOps.add(other.getData()->r, vOps.scale(data.r, -1));
    vec colNorm = vOps.normalise(col);
    vec relVel = vOps.add(other.getData()->vel, vOps.scale(data.vel, -1));
    float velNorm = vOps.dot(relVel, colNorm);
    if (velNorm > 0) return;
    float e = 0.6f;
    float j = -(1 + e) * velNorm / (1 / data.mass + 1 / other.getData()->mass);
    vec impulse = vOps.scale(colNorm, j);
    data.vel = vOps.add(data.vel, vOps.scale(impulse, -1 / data.mass));
    data.impulse = vOps.scale(impulse, -1);
    other.getData()->vel = vOps.add(other.getData()->vel, vOps.scale(impulse, 1 / other.getData()->mass));
    other.getData()->impulse = impulse;
    if (other.getData()->floor) {
        other.getData()->vel = vOps.scale(other.getData()->vel, -1);
    }
}

ObjectData* Object::getData() {
    return &data;
}

float Sphere::SDF(vec p, vec c, float r) {
    return vOps.length(vOps.add(c, vOps.scale(p, -1))) - r;
}