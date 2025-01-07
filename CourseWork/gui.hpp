#pragma once

#include "imgui.h"
#include "object.hpp"
#include "material.hpp"
#include "filehandler.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <regex>
#include <filesystem>

class GUI {
private:
    int width, height, actions[30], redo, current = 0;
    void clampFloat(float& f, float min, float max);
    std::string serializeMaterials(const std::vector<Material>& materials);
    std::string serializeObjects(const std::vector<std::shared_ptr<Object>>& objects);
    std::string serializeScene(const std::vector<Material>& materials, const std::vector<std::shared_ptr<Object>>& objects);
    void deserialize(std::string data, std::vector<Material>& materials, std::vector<std::shared_ptr<Object>>& objects);
public:
    GUI(int width, int height);
    void screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, std::vector<Material>& materials, float &r, float &f, float &g, float fps, std::vector<std::string> tArr, std::vector<std::string> tMatArr, std::vector<std::vector<float>> lights, std::vector<std::vector<float>> lightCols);
};
