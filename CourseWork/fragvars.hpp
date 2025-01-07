#pragma once

#include <GL/glew.h>
#include <vector>
#include <chrono>
#include <memory>

#include "material.hpp"
#include "object.hpp"
#include "programH.hpp"
#include "uniformbuffer.hpp"

class FragVars {
private:
    std::vector<float> res;
    float aX, aY;
    std::vector<std::vector<float>> lights, lightCols;
    std::vector<Material> materials;
    std::vector<std::shared_ptr<Object>> objects;
    std::chrono::steady_clock::time_point time;
    GLint resLocation, aYLocation, aXLocation, timeLocation, lightsLocation, lightColsLocation,
        materialsLocation, objectsLocation, lightsLLocation, materialsLLocation, objectsLLocation;
    UniformBuffer ub;

    void materialObjectDataUpdate(Program& p);
public:
    FragVars(std::vector<float>& res, float aX, float aY, std::vector<std::vector<float>>& lights,
        std::vector<std::vector<float>>& lightCols, std::vector<Material>& materials, std::vector<std::shared_ptr<Object>>& objects);

    void init(Program &p);

    void update(Program& p, const float &aX, const float &aY, std::vector<Material> materials, std::vector<std::shared_ptr<Object>>& objects);
};