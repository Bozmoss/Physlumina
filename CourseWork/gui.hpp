#pragma once

#include "imgui.h"
#include "object.hpp"
#include "material.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>

class GUI {
private:
    int width, height;
    void clampFloat(float& f, float min, float max);
public:
    GUI(int width, int height);
    void mainMenu(int& screen);
    void screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, std::vector<Material>& materials, float fps, float &g, std::vector<std::string> tArr, std::vector<std::string> tMatArr);
};
