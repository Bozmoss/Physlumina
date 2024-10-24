#pragma once

#include "imgui.h"
#include "object.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>

class GUI {
private:
    int width, height;
public:
    GUI(int width, int height);
    void mainMenu(int& screen);
    void screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, float fps, float &g, std::vector<std::string> tArr);
};
