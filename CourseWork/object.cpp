#include "object.hpp"

#include <iostream>

Object::Object(ObjectData data):
    data{ data }, 
    lastT{ 0 } {}

float Object::SDF() { return 0; }

void Object::update(float f) {
    if (data.r.y - data.l1 <= -1.5 && !data.moving) {
        data.vel.x *= f;
        data.vel.z *= f;
    }
    data.r = vOps.add(data.r, data.vel);
}

void Object::updateObject(float g, float r) {
    if (data.floor) {
        data.vel.y = 0.0f;
        data.floor = false;
        data.down = false;
    }
    if (!data.moving) {
        if (data.down) {
            data.vel.y -= g;
            if (data.r.y - data.l1 < -1.5) {
                data.r.y = -1.5 + data.l1;
                data.vel.y = -data.vel.y * r;
                if (abs(data.vel.y) < g * 0.25) {
                    data.floor = true;
                }
            }
        }
        else {
            data.vel.y -= g;
            if (data.vel.y <= 0) {
                data.down = true;
            }
        }
    }
}

void Object::stopMoving() {
    data.moving = false;
}

bool Object::isClicked(std::vector<float> ro, std::vector<float> rd, float finalT) {
    return false;
}

bool Object::checkCollision(Object& other) {
    float dist = vOps.length(vOps.add(data.r, vOps.scale(other.getData()->r, -1)));

    std::cout << dist << std::endl;

    return dist < (data.l1 + other.getData()->l1);
}

void Object::resolveCollision(Object& other) {
    vec colNorm = vOps.normalise(vOps.add(other.getData()->r, vOps.scale(data.r, -1)));
    vec relVel = vOps.add(other.getData()->vel, vOps.scale(data.vel, -1));
    float velNorm = vOps.dot(relVel, colNorm);
    if (velNorm > 0) return;
    float e = 0.6f;
    float j = -(1 + e) * velNorm / (1 / data.mass + 1 / other.getData()->mass);
    vec impulse = vOps.scale(colNorm, j);
    data.vel = vOps.add(data.vel, vOps.scale(impulse, -1 / data.mass));
    other.getData()->vel = vOps.add(other.getData()->vel, vOps.scale(impulse, 1 / other.getData()->mass));

    float overlap = data.l1 + other.getData()->l1 - vOps.length(vOps.add(data.r, vOps.scale(other.getData()->r, -1)));
    vec correction = vOps.scale(colNorm, overlap / 2.0f);
    data.r = vOps.add(data.r, correction);
    other.getData()->r = vOps.add(other.getData()->r, vOps.scale(correction, -1));
}

float Object::getLastT() {
    return lastT;
}

void Object::reset() {
    lastT = 0;
}

ObjectData *Object::getData() {
    return &data;
}


float Sphere::SDF(vec p, vec c, float r) {
    return vOps.length(vOps.add(c, vOps.scale(p, -1))) - r;
}

bool Sphere::isClicked(std::vector<float> ro, std::vector<float> rd, float finalT) {
    float t = 0.0f;
    for (int i = 0; i < 100; i++) {
        vec p = {
            ro.at(0) + t * rd.at(0),
            ro.at(1) + t * rd.at(1),
            ro.at(2) + t * rd.at(2)
        };
        float sdf = SDF(p, data.r, data.l1);
        //std::cout << "SDF at t = " << t << ": " << sdf << "\n";
        if (sdf < 0.001f && t < finalT) {
            lastT = t;
            return true;
        }
        t += sdf;
        if (t > 100.0f) {
            lastT = t;
            break;
        }
    }
    return false;
}