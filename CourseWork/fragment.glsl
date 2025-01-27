#version 440 core
#define lightsLen 100
#define materialsLen 100
#define objectsLen 100

out vec4 FragColor;
uniform vec3 res, lights[lightsLen], lightCols[lightsLen];
uniform float time, aX, aY;
uniform int lightsL, materialsL, objectsL;

struct Material {
    float r;
    float g;
    float b;
    float Ka;
    float Kd;
    float Ks;
    float Kr;
    float c;
};

struct ObjectData {
    float type;
    float material;
    float x;
    float y;
    float z;
    float l1;
    float br;
};

layout(std140) uniform bindPoint {
    Material materials[materialsLen];
    ObjectData objects[objectsLen];
};

struct SDF {
    float dist;
    int index;
};

bool intersectsBoundingVolume(vec3 ro, vec3 rd, vec3 c, float r) {
    vec3 oc = ro - c;
    float b = dot(oc, rd);
    float c1 = dot(oc, oc) - r * r;
    return b * b - c1 >= 0.0;
}

float dynamicStepSize(float dist, float minStep, float maxStep) {
    return clamp(dist, minStep, maxStep);
}

SDF sphereSDF(vec3 p, vec3 c, float r, int i) {
    return SDF(length(c-p) - r, i);
}

SDF planeSDF(vec3 p, vec3 n, float h, int i) {
  return SDF(dot(p, n) + h, i);
}

SDF unionSDF(SDF SDF1, SDF SDF2) {
    return (SDF1.dist < SDF2.dist) ? SDF1 : SDF2;
}

SDF intersectSDF(SDF SDF1, SDF SDF2) {
    return (SDF1.dist > SDF2.dist) ? SDF1 : SDF2;
}

SDF subSDF(SDF SDF1, SDF SDF2) {
    if (SDF1.dist > -SDF2.dist) {
        return SDF1;
    }
    return SDF2;
}

mat3 rotateX(float a) {
    float s = sin(a), c = cos(a);
    return mat3(
    1.0, 0.0, 0.0,
    0.0, c, -s,
    0.0, s, c);
}

mat3 rotateY(float a) {
    float s = sin(a), c = cos(a);
    return mat3(
    c, 0.0, s,
    0.0, 1.0, 0.0,
    -s, 0.0, c);
}

vec3 rotateSDF(vec3 p, float x, float y) {
    mat3 rotX = rotateX(x);
    mat3 rotY = rotateY(y);
    return p * rotY * rotX;
}

vec3 translateSDF(vec3 p, vec3 t) {
    return p - t;
}

SDF finalSDF(vec3 p, vec3 rd) {
    SDF final = planeSDF(p, vec3(0.0, 1.0, 0.0), 1.5, 0);
    for (int i = 0; i < objectsL; i++) {

        if (!intersectsBoundingVolume(p, rd, vec3(objects[i].x, objects[i].y, objects[i].z), objects[i].l1)) {
            continue;
        }

        SDF temp;
        switch(int(objects[i].type)) {
            case 0:
                temp = sphereSDF(p, vec3(objects[i].x, objects[i].y, objects[i].z), objects[i].l1, int(objects[i].material));
                break;
            case 1:
                temp = planeSDF(p, vec3(objects[i].x, objects[i].y, objects[i].z), objects[i].l1, int(objects[i].material));
                break;
        };
        final = unionSDF(final, temp);
    }
    return final;
}

vec3 calculateNormal(vec3 p, vec3 rd) {
    const float eps = 0.0001;
    const vec2 epsVec = vec2(eps, 0);
    return normalize(vec3(
        finalSDF(p+epsVec.xyy, rd).dist - finalSDF(p-epsVec.xyy, rd).dist,
        finalSDF(p+epsVec.yxy, rd).dist - finalSDF(p-epsVec.yxy, rd).dist,
        finalSDF(p+epsVec.yyx, rd).dist - finalSDF(p-epsVec.yyx, rd).dist
    ));
}

bool isInShadow(vec3 p, vec3 rd, float dist) {
    float t = 0.01;
    for (int i = 0; i < 100; i++) {
        vec3 pos = p + t * rd;
        SDF res = finalSDF(pos, rd);
        if (res.dist < 0.001) {
            return true;
        }
        t += dynamicStepSize(res.dist, 0.001, 1.0);
        if (t >= dist) {
            return false;
        }
    }
    return false;
}

vec3 getCol(vec3 Ia, Material m, vec3 Ii[lightsLen], vec3 n, vec3 li[lightsLen], vec3 v, vec3 p) {
    vec3 a = Ia * m.Ka;
    vec3 d = vec3(0.0);
    vec3 s = vec3(0.0);
    for (int i = 0; i < lightsL; i++) {
        vec3 light = normalize(li[i] - p);
        float dist = length(light);
        if (!isInShadow(p, light, dist)) {
            vec3 r = reflect(-light, n);
            d += max(dot(n, light), 0.0) * m.Kd * Ii[i] * vec3(m.r, m.g, m.b);
            s += m.Ks * pow(max(dot(r, normalize(v)), 0.0), m.c) * Ii[i];
        }
    }
    return a + d + s;
}

vec2 rayMarch(vec3 ro, vec3 rd, float maxDist) {
    float t = 0.0;
    for (int i = 0; i < 100; i++) {
        vec3 pos = ro + t * rd;
        SDF res = finalSDF(pos, rd);
        if (res.dist < 0.001) {
            return vec2(t, res.index);
        }
        t += dynamicStepSize(res.dist, 0.001, 1.0);
        if (t > maxDist) break;
    }
    return vec2(t, -1);
}

vec3 checkerFloor(vec3 p, vec3 n) {
    float scale = 1.0;
    int checker = int(floor(p.x * scale) + floor(p.z * scale)) % 2;
    vec3 baseColor = (checker == 0) ? vec3(118.0f /  255.0f, 150.0f / 255.0f, 86.0f / 255.0f) : vec3(238.0f / 255.0f, 238.0f / 255.0f, 210.0f / 255.0f);
    vec3 shadowColor = vec3(0.5);
    for (int i = 0; i < lightsL; i++) {
        vec3 lightDir = normalize(lights[i] - p);
        float distToLight = length(lights[i] - p);

        if (isInShadow(p + n * 0.01, lightDir, distToLight)) {
            return mix(baseColor, shadowColor, 0.5);
        }
    }

    return baseColor;
}

vec3 sortCol(vec3 ro, vec3 rd, float maxDist) {
    vec3 c = vec3(0.0, 0.0, 0.0);
    float Rf = 1.0;

    for (int depth = 0; depth < 3; depth++) {
        vec2 t = rayMarch(ro, rd, maxDist);
        vec3 pos = ro + t.x * rd;
        if (t.x < maxDist && t.y >= 0) {
            vec3 n = calculateNormal(pos, rd);
            vec3 view = normalize(ro - pos);
            vec3 Ia = vec3(1.0);
            vec3 surfaceC = getCol(Ia, materials[int(t.y)], lightCols, n, lights, view, pos);

            if (int(t.y) == 0) {
                surfaceC = checkerFloor(pos, vec3(0.0, 1.0, 0.0));
                float Ff = clamp(1.0 - (length(pos - ro) / maxDist), 0.0, 1.0);
                surfaceC = mix(c, surfaceC, Ff);
            }

            c += Rf * surfaceC;
            Rf *= materials[int(t.y)].Kr;

            rd = reflect(rd, n);
            ro = pos + n * 0.01;
        } else {
            break;
        }
    }
    return c;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / res.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= res.x / res.y;

    vec3 ro = vec3(0.0, 0.0, 2.0);
    vec3 rd = normalize(vec3(uv, -1.0));
    mat3 rotX = rotateX(aX);
    mat3 rotY = rotateY(aY);
    ro = ro * rotX * rotY;
    rd = rd * rotX * rotY;

    float maxDist = 10.0;

    vec3 col = sortCol(ro, rd, maxDist);

    FragColor = vec4(col, 1.0);
}