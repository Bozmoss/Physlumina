#include "fragvars.hpp"

FragVars::FragVars(std::vector<float> &res, float aX, float aY, std::vector<std::vector<float>> &lights, std::vector<std::vector<float>> &lightCols, std::vector<Material> &materials, std::vector<std::shared_ptr<Object>> &objects) {
    this->res = res;
    this->aX = aX;
    this->aY = aY;
    this->lights = lights;
    this->lightCols = lightCols;
    this->materials = materials;
    this->objects = objects;
}

void FragVars::init(Program &p) {
    materialObjectDataUpdate(p);
    resLocation = glGetUniformLocation(p.handle(), "res");
    aYLocation = glGetUniformLocation(p.handle(), "aY");
    aXLocation = glGetUniformLocation(p.handle(), "aX");
    timeLocation = glGetUniformLocation(p.handle(), "time");
    lightsLocation = glGetUniformLocation(p.handle(), "lights");
    lightColsLocation = glGetUniformLocation(p.handle(), "lightCols");
    lightsLLocation = glGetUniformLocation(p.handle(), "lightsL");
    materialsLLocation = glGetUniformLocation(p.handle(), "materialsL");
    objectsLLocation = glGetUniformLocation(p.handle(), "objectsL");
    time = std::chrono::high_resolution_clock::now();
}
void FragVars::update(Program& p, const float &aX, const float &aY, std::vector<Material> materials, std::vector<std::shared_ptr<Object>>& objects, std::vector<std::vector<float>>& lights, std::vector<std::vector<float>>& lightCols) {
    this->materials = materials;
    this->objects = objects;
    this->aX = aX;
    this->aY = aY;
    this->lights = lights;
    this->lightCols = lightCols;
    materialObjectDataUpdate(p);
    glUniform3f(resLocation, res.at(0), res.at(1), res.at(2));
    glUniform1f(aXLocation, this->aX);
    glUniform1f(aYLocation, this->aY);
    glUniform1i(lightsLLocation, lights.size());
    glUniform1i(materialsLLocation, materials.size());
    glUniform1i(objectsLLocation, objects.size());
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - time;
    glUniform1f(timeLocation, elapsed.count());
    GLfloat lightsArr[100][3], lightColsArr[100][3];
    for (int i = 0; i < 100; i++) {
        if (i < lights.size()) {
            for (int j = 0; j < 3; j++) {
                lightsArr[i][j] = lights.at(i).at(j);
            }
        }
        else {
            for (int j = 0; j < 3; j++) {
                lightsArr[i][j] = 0;
            }
        }
        if (i < lightCols.size()) {
            for (int j = 0; j < 3; j++) {
                lightColsArr[i][j] = lightCols.at(i).at(j);
            }
        }
        else {
            for (int j = 0; j < 3; j++) {
                lightColsArr[i][j] = 0;
            }
        }
    }
    glUniform3fv(lightsLocation, 100, &lightsArr[0][0]);
    glUniform3fv(lightColsLocation, 100, &lightColsArr[0][0]);
}

void FragVars::materialObjectDataUpdate(Program& p) {
    std::vector<float> result;
    for (int i = 0; i < 100; i++) {
        if (i < materials.size()) {
            result.push_back(materials.at(i).r);
            result.push_back(materials.at(i).g);
            result.push_back(materials.at(i).b);
            result.push_back(materials.at(i).Ka);
            result.push_back(materials.at(i).Kd);
            result.push_back(materials.at(i).Ks);
            result.push_back(materials.at(i).Kr);
            result.push_back(materials.at(i).c);
        }
        else {
            for (int j = 0; j < 8; j++) {
                result.push_back(0.0);
            }
        }
    }
    for (int i = 0; i < 100; i++) {
        if (i < objects.size()) {
            result.push_back(objects.at(i)->getData()->type);
            result.push_back(objects.at(i)->getData()->material);
            result.push_back(objects.at(i)->getData()->r.x);
            result.push_back(objects.at(i)->getData()->r.y);
            result.push_back(objects.at(i)->getData()->r.z);
            result.push_back(objects.at(i)->getData()->l1);
            result.push_back(0.0f);
            result.push_back(0.0f);
        }
        else {
            for (int j = 0; j < 8; j++) {
                result.push_back(0.0f);
            }
        }
    }
    ub.fill(result, p, "bindPoint");
}