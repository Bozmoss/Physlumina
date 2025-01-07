#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "GLOBAL.hpp"
#include "object.hpp"
#include "vec.hpp"

class Game {
private:

    struct Point {
        float x, y;
    };

    float aX = 0, aY = 0, lastX, lastY, g = 0.00003, r = 0.8, f = 0.7;
    bool firstMouse = true;
    std::vector<std::shared_ptr<Object>> objects;

    Point screenToNDC(float x, float y);

public:

    Game(std::vector<std::shared_ptr<Object>> &objects);

    void mouseEvent(GLFWwindow* window, double xpos, double ypos, float g, float r);

    void updateObjects(std::vector<std::shared_ptr<Object>>& objects);

    float getAX(), getAY();
};
