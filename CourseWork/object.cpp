#include "object.hpp"

#include <iostream>

Object::Object(ObjectData data):
    data{ data }, 
    lastT{ 0 } {}

float Object::SDF(vec a, vec b, float c) { return 0; }

float Object::max(float a, float b) {
    return (a < b) ? b : a;
}

float Object::min(float a, float b) {
    return !(a < b) ? a : b;
}

void Object::update(float f, float g, float r) {
    if (data.r.y - data.l1 > -1.5f) {
        vec gravity = { 0.0, -g, 0.0 };
        data.vel = vOps.add(data.vel, gravity);
        data.r = vOps.add(data.r, data.vel);
    }
    else {
        data.r.y = data.l1 - 1.5f;
        data.vel.y = 0;
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
            vec r1 = data.r;
            vec r2 = other.getData()->r;

            vec v1 = data.vel;
            vec v2 = other.getData()->vel;

            vec r1_to_r2 = vOps.add(r2, vOps.scale(r1, -1));  // Vector from cube 1 to cube 2
            vec v_rel = vOps.add(v2, vOps.scale(v1, -1));      // Relative velocity

            // Step 2: Calculate normal vector (axis of collision)
            vec n = vOps.normalise(r1_to_r2);

            // Step 3: Calculate normal and tangential components of relative velocity
            vec v_rel_normal = vOps.scale(n, vOps.dot(v_rel, n));
            vec v_rel_tangent = vOps.add(v_rel, vOps.scale(vOps.scale(n, vOps.dot(v_rel, n)), -1));  // Subtract normal component

            // Step 4: Calculate the impulse using the impulse-momentum theorem
            float e = 0.8f;  // Coefficient of restitution (elasticity)
            float m1 = data.mass;
            float m2 = other.getData()->mass;

            // Calculate the moment arms (r_1 and r_2) from the centers of mass
            vec r1_contact = vOps.add(r1, vOps.scale(n, data.l1));  // Assume center of mass and contact point
            vec r2_contact = vOps.add(r2, vOps.scale(n, other.getData()->l1));

            // Moments of inertia (assuming uniform cubes, calculate based on mass and dimensions)
            float I1 = data.inertia;  // These would be computed based on the shape and mass distribution
            float I2 = other.getData()->inertia;

            // Calculate the impulse magnitude
            float impulse_magnitude = -(1 + e) * vOps.dot(v_rel, n) / (1 / m1 + 1 / m2 + vOps.dot(r1_contact, n) / I1 + vOps.dot(r2_contact, n) / I2);

            // Step 5: Apply impulse to both cubes' velocities
            vec impulse = vOps.scale(n, impulse_magnitude);

            data.vel = vOps.add(data.vel, vOps.scale(impulse, 1 / m1));
            other.getData()->vel = vOps.add(other.getData()->vel, vOps.scale(impulse, -1 / m2));

            // Step 6: Apply torque (change in angular velocity)
            vec torque1 = vOps.cross(r1_contact, impulse);
            vec torque2 = vOps.cross(r2_contact, impulse);

            data.angVel = vOps.add(data.angVel, vOps.scale(torque1, 1 / I1));
            other.getData()->angVel = vOps.add(other.getData()->angVel, vOps.scale(torque2, 1 / I2));
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

float Box::SDF(vec p, vec c, float bL) {
    vec b = { bL, bL, bL };
    vec translated = vOps.add(p, vOps.scale(c, -1));
    vec q = vOps.add({ abs(translated.x), abs(translated.y), abs(translated.z) }, vOps.scale(b, -1));
    return vOps.length({max(q.x, 0.0), max(q.y, 0.0), max(q.z, 0.0)}) + min(max(q.x, max(q.y, q.z)), 0.0);
}