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
            //o->stopMoving();
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
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
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
    }
    else {
        Point pos = screenToNDC(xpos, ypos);
        Point last = screenToNDC(lastX, lastY);

        std::vector<float> posStart = {
            0.0, 0.0, -5
        };

        float fovY = 90.0 * (PI / 180.0);
        float aspectR = WINDOW_WIDTH / WINDOW_HEIGHT;
        float fovX = 2 * atan(tan(fovY / 2.0) * aspectR);
        float viewX = pos.x * tan(fovX / 2.0);
        float viewY = pos.y * tan(fovY / 2.0);

        std::vector<float> posEnd = {
            viewX, viewY, -1
        };

        float mag = sqrt(
            (posEnd[0] - posStart[0]) * (posEnd[0] - posStart[0]) +
            (posEnd[1] - posStart[1]) * (posEnd[1] - posStart[1]) +
            (posEnd[2] - posStart[2]) * (posEnd[2] - posStart[2])
        );

        std::vector<float> posRd = {
            (posEnd[0] - posStart[0]) / mag,
            (posEnd[1] - posStart[1]) / mag,
            (posEnd[2] - posStart[2]) / mag
        };

        xoffset = pos.x - last.x;
        yoffset = pos.y - last.y;
        xoffset *= sensitivity * 200;
        yoffset *= sensitivity * 200;
        if (abs(xoffset) < 0.001 && abs(yoffset) < 0.001) {
            xoffset = 0;
            yoffset = 0;
        }

        float finalT = 11.0;
        int finalIndex = -1;
        int i = 0;
        for (auto& o : objects) {
            if (o->isClicked(posStart, posRd, finalT)) {
                finalT = o->getLastT();
                finalIndex = i;
            }
            i++;
        }
        if (finalIndex != -1) {
            objects.at(finalIndex)->getData()->moving = true;
            objects.at(finalIndex)->getData()->vel.x = xoffset;
            objects.at(finalIndex)->getData()->vel.y = yoffset;
        }
        for (auto& o : objects) {
            if (o->getData()->moving) {
                if (xoffset != 0 || yoffset != 0) {
                    o->getData()->vel.x = xoffset;
                    o->getData()->vel.y = yoffset;
                }
                else {
                    o->getData()->vel.x = 0;
                    o->getData()->vel.y = 0;
                }
            }
            else {
                o->updateObject(g, r);
            }
        }
        lastX = xpos;
        lastY = ypos;
    }
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

//TODO: Add rigid body dynamics
// - add to object struct vel, angularVel, mass
// - update(): apply forces as vectors -> vector class?
// - if vector class then change object struct to use vector class
// - resolveCollision(): use collision normal, relativeVel (along normal) (> 0 return), e coeff, j = -(1+e) * velalongnorm / (1 / m1 + 1/ m2)
//   impulse = j * norm, vel1 -= imp/m, vel2 += imp/m
// - collision if SDF at possible point of contact < 0 (posibly)
//TODO: Improve mouse detection
//TODO: Add UI and main menus as described in design
