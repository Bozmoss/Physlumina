#include "game.hpp"

#include <iostream>

Game::Point Game::screenToNDC(float x, float y) {
    Point ndc;
    ndc.x = 2.0f * x / WINDOW_WIDTH - 1.0f;
    ndc.y = 1.0f - 2.0f * y / WINDOW_HEIGHT;
    return ndc;
}

Game::Game(std::vector<std::shared_ptr<Object>> &objects):
    objects { objects } {}

void Game::mouseEvent(GLFWwindow* window, double xpos, double ypos, float g, float r) {
    float xoffset = 0, yoffset = 0;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        for (auto& o : objects) {
            o->reset();
        }
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float sensitivity = 0.01f;
    
    xoffset = xpos - lastX;
    yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    aX += yoffset;
    aY -= xoffset;

    if (aX > PI / 4)
        aX = PI / 4;
    if (aX < -PI / 2)
        aX = -PI / 2;
    
    for (auto& o : objects) {
        if (o->getData()->moving) {
            if (xoffset == 0 || yoffset == 0) {
                o->getData()->vel.x = 0;
                o->getData()->vel.y = 0;
            }
        }
    }
}

void Game::updateObjects(std::vector<std::shared_ptr<Object>>& objects) {
    this->objects = objects;
}

float Game::getAX() {
    return aX;
}

float Game::getAY() {
    return aY;
}
