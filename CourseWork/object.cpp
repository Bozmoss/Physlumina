#include "object.hpp"

#include <iostream>

quar hProd(quar q1, quar q2) {
    return {
        q1.w * q2.w - q1.i * q2.i - q1.j * q2.j - q1.k * q2.k,
        q1.w * q2.i + q1.i * q2.w - q1.j * q2.k + q1.k * q2.j,
        q1.w * q2.j + q1.i * q2.k + q1.j * q2.w - q1.k * q2.i,
        q1.w * q2.k - q1.i * q2.j + q1.j * q2.i + q1.k * q2.w
    };
}

Object::Object(ObjectDataFull data):
    data{ data }, 
    lastT{ 0 } {
    necData = { data.type, data.material, data.r, data.angVel, data.l1 };
}

float Object::SDF(vec a, vec b, float c) { return 0; }

float Object::max(float a, float b) {
    return (a < b) ? b : a;
}

float Object::min(float a, float b) {
    return !(a < b) ? a : b;
}

void Object::update(float f, float g, float r, bool colliding) {
    if (!colliding) {
        if (data.r.y - data.l1 > -1.5f) {
            vec gravity = { 0.0, -g, 0.0 };
            data.vel = vOps.add(data.vel, gravity);
            data.r = vOps.add(data.r, data.vel);
            necData.r = vOps.add(data.r, data.vel);
        }
        else {
            data.r.y = data.l1 - 1.5f;
            necData.r.y = data.l1 - 1.5f;
            data.vel.y = 0;
        }

        quar pos = { 1, data.r.x, data.r.y, data.r.z };
        quar ome = { 1, data.angVel.x, data.angVel.y, data.angVel.z };
        pos = hProd(pos, ome);
        data.r = { pos.i, pos.j, pos.k };
        necData.r = { pos.i, pos.j, pos.k };
    }
}

bool Object::isClicked(std::vector<float> ro, std::vector<float> rd, float finalT) {
    return false;
}

bool Object::checkCollision(Object& other) {
    switch (data.type) {
    case 0:
        switch (other.getData()->type) {
        case 0:
        {
            float d = abs(vOps.length(vOps.add(data.r, vOps.scale(other.getData()->r, -1))));
            d *= d;
            return d <= (data.l1 + other.getData()->l1) * (data.l1 + other.getData()->l1);
            break;
        }
        case 1:
        {
            return SBCheck(*this, other);
            break;
        }
        }
        break;
    case 1:
        switch (other.getData()->type) {
        case 0:
        {
            return SBCheck(other, *this);
            break;
        }
        case 1:
        {
            vec box1Min = vOps.add(data.r, vOps.scale({ -data.l1, -data.l1, -data.l1 }, 1));
            vec box1Max = vOps.add(data.r, vOps.scale({ data.l1, data.l1, data.l1 }, 1));
            vec box2Min = vOps.add(other.getData()->r, vOps.scale({ -other.getData()->l1, -other.getData()->l1, -other.getData()->l1 }, 1));
            vec box2Max = vOps.add(other.getData()->r, vOps.scale({ other.getData()->l1, other.getData()->l1, other.getData()->l1 }, 1));
            if (!overlapOnAxis(box1Min.x, box1Max.x, box2Min.x, box2Max.x)) return false;
            if (!overlapOnAxis(box1Min.y, box1Max.y, box2Min.y, box2Max.y)) return false;
            if (!overlapOnAxis(box1Min.z, box1Max.z, box2Min.z, box2Max.z)) return false;
            return true;
            break;
        }
        }
        break;
    }
}

bool Object::overlapOnAxis(float min1, float max1, float min2, float max2) {
    return !(max1 < min2 || max2 < min1);
}

bool Object::SBCheck(Object s, Object b) {
    vec closest = clampToBox(s.getData()->r,
        vOps.add(b.getData()->r, vOps.scale({ b.getData()->l1, b.getData()->l1, b.getData()->l1 }, -1)),
        vOps.add(b.getData()->r, { b.getData()->l1, b.getData()->l1, b.getData()->l1 }));
    float d = vOps.length(vOps.add(s.getData()->r, vOps.scale(closest, -1)));
    d *= d;
    return d < (s.getData()->l1 * s.getData()->l1);
}

vec Object::clampToBox(vec p, vec bMin, vec bMax) {
    return {
        vOps.clamp(p.x, bMin.x, bMax.x),
        vOps.clamp(p.y, bMin.y, bMax.y),
        vOps.clamp(p.z, bMin.z, bMax.z)
    };
}

void Object::resolveCollision(Object& other) {
    switch (data.type) {
    case 0:
        switch (other.getData()->type) {
        case 0:
        {
            break;
        }
        case 1:
        {
            break;
        }
        }
        break;
    case 1:
        switch (other.getData()->type) {
        case 0:
        {
            break;
        }
        case 1:
        {
            data.vel = { 0,0,0 };
            break;
        }
        }
        break;
    }
}

float Object::getLastT() {
    return lastT;
}

void Object::reset() {
    lastT = 0;
}

ObjectData *Object::getNecessaryData() {
    return &necData;
}

ObjectDataFull* Object::getData() {
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

float Box::SDF(vec p, vec c, float bL) {
    vec b = { bL, bL, bL };
    vec translated = vOps.add(p, vOps.scale(c, -1));
    vec q = vOps.add({ abs(translated.x), abs(translated.y), abs(translated.z) }, vOps.scale(b, -1));
    return vOps.length({max(q.x, 0.0), max(q.y, 0.0), max(q.z, 0.0)}) + min(max(q.x, max(q.y, q.z)), 0.0);
}