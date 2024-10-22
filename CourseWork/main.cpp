/*****************************************************************//**
 * \file   main.cpp
 * \brief  3D rendering engine
 * 
 * \author Ben
 * \date   August 2024
 *********************************************************************/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <unordered_map>
#include <functional>

#include "GLOBAL.hpp"
#include "filehandler.hpp"
#include "shader.hpp"
#include "programH.hpp"
#include "vertexbuffer.hpp"
#include "indexbuffer.hpp"
#include "fragvars.hpp"
#include "material.hpp"
#include "object.hpp"
#include "game.hpp"
#include "vec.hpp"
#include "gui.hpp"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ (hash2 << 1);
    }
};

std::vector<Material> materials;
std::vector<std::shared_ptr<Object>> objects;
std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<Object>>, pair_hash> spatialHash;
Game game(objects);
int screen = 0;
float g = 0.000075, r = 0.8, f = 0.7;
const int bound = 10;
const float gridSize = 1.0/10.0f;

std::pair<int, int> computeHash(const vec& pos) {
    int x = static_cast<int>(pos.x / gridSize);
    int y = static_cast<int>(pos.y / gridSize);
    std::cout << "Computed Hash Key: (" << x << ", " << y << ")" << std::endl;
    return { x, y };
}

void mouse(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        game.mouseEvent(window, xpos, ypos, g, r);
    }
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwDestroyWindow(window);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        objects.clear();
        for (int i = 0; i < bound; i++) {
            int r = rand() % 1000, r2 = rand() % 1000;
            ObjectData o = { 0, 0, vec {r / 1000.0f, (float)i / 5, r2 / 1000.0f}, 0.1, 1.0 };
            auto s = std::make_shared<Sphere>(o);
            objects.push_back(s);
        }
    }
}

void printSpatialHash() {
    for (const auto& pair : spatialHash) {
        const auto& hashKey = pair.first; // pair.first is the key (x, y)
        const auto& cellObjects = pair.second; // pair.second is the vector of objects in that cell

        std::cout << "Hash Key: (" << hashKey.first << ", " << hashKey.second << "): ";

        // Print object details
        for (const auto& obj : cellObjects) {
            std::cout << "Object at ("
                << obj->getData()->r.x << ", "
                << obj->getData()->r.y << ", "
                << obj->getData()->r.z << ") with radius "
                << obj->getData()->l1 << " | ";
        }
        std::cout << std::endl; // New line after each hash bucket
    }
}

void update() {
    for (auto& o : objects) {
        // Store old hash key based on object's previous position
        auto oldHashKey = computeHash(o->getData()->r);

        // Update object (apply forces, move object, etc.)
        o->updateObject(g, r);

        // Compute new hash key based on object's new position
        auto newHashKey = computeHash(o->getData()->r);

        // Only rehash the object if it has moved to a different grid cell
        if (oldHashKey != newHashKey) {
            // Remove object from the old cell
            auto& oldCell = spatialHash[oldHashKey];
            oldCell.erase(std::remove(oldCell.begin(), oldCell.end(), o), oldCell.end());

            // Add object to the new cell
            spatialHash[newHashKey].push_back(o);
        }
    }

    printSpatialHash();
    // Check for collisions in each cell
    for (auto& pair : spatialHash) {
        auto& cellObjects = pair.second;
        for (size_t i = 0; i < cellObjects.size(); i++) {
            for (size_t j = i + 1; j < cellObjects.size(); j++) {
                if (cellObjects[i]->checkCollision(*cellObjects[j])) {
                    std::cout << "Collision detected between objects " << i << " and " << j << std::endl;
                    cellObjects[i]->resolveCollision(*cellObjects[j]);
                }
            }
        }
    }
}

void updateObjectDatas() {
    for (auto& o : objects) {
        o->update(f);
    }
    game.updateObjects(objects);
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW";
        return -1;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (mode == nullptr) {
        std::cerr << "Failed to retrieve video mode" << std::endl;
        glfwTerminate();
        return -1;
    }

    g = 3*mode->refreshRate / 1000000.0 * bound / 3;
    r = 3*mode->refreshRate / 330.0;

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key);
    glfwSetCursorPosCallback(window, mouse);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialise GLEW";
        glfwTerminate();
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440");

    Shader vertexShader(Shader::Type::VERTEX, "./vertex.glsl");
    if (vertexShader.inError()) {
        std::cout << vertexShader.error() << "\n";
    }
    Shader fragmentShader(Shader::Type::FRAGMENT, "./fragment.glsl");
    if (fragmentShader.inError()) {
        std::cout << fragmentShader.error() << "\n";
    }

    Program p;
    p.attachShader(vertexShader);
    p.attachShader(fragmentShader);
    p.activate();

    std::vector<float> res = { (float)mode->width, (float)mode->height, 1.0f };

    std::vector<std::vector<float>> lights = { //DEFAULT
        {2.0, 0.6, 1.0},
        {-2.0, 0.6, 1.0},
        {0.0, 0.8, 1.0}
    };

    std::vector<std::vector<float>> lightCols = { //DEFAULT
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0}
    };

    materials = {
    {1.0f, 1.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.2f, 32.0f},
    {0.0f, 1.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.4f, 32.0f},
    {0.0f, 0.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {0.0f, 0.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.4f, 32.0f},
    };

    FragVars fvs(res, game.getAX(), game.getAY(), lights, lightCols, materials, objects);

    std::vector<GLfloat> verts = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    VertexBuffer vB;
    vB.fill(verts);

    std::vector<GLuint> indices = {
        0, 1, 2, 2, 1, 3
    };

    IndexBuffer iB;
    iB.fill(indices);

    auto vertexPosition = glGetAttribLocation(p.handle(), "vertexPosition");
    glEnableVertexAttribArray(vertexPosition);
    glBindBuffer(GL_ARRAY_BUFFER, vB.handle());
    glVertexAttribPointer(vertexPosition, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    fvs.init(p);

    GUI gui(mode->width, mode->height);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        switch (screen) {
        case 0:
            gui.mainMenu(screen);
            break;
        case 1:
            update();
            updateObjectDatas();

            p.activate();
            fvs.update(p, game.getAX(), game.getAY(), materials, objects);

            // Clear the screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDrawElements(GL_TRIANGLES, iB.number(), GL_UNSIGNED_INT, nullptr);

            gui.screenOne(window, objects);
            break;
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}




/*std::vector<std::vector<float>> lights = {
    {2.0, 0.6, 1.0},
    {-2.0, 0.6, 1.0},
    {0.0, 0.8, 1.0}
};*/ //PRESET LIGHT POSITIONS
/*std::vector<std::vector<float>> lightCols = {
    {1.0, 1.0, 1.0},
    {1.0, 1.0, 1.0},
    {1.0, 1.0, 1.0}
};*/ //PRESET LIGHT COLOURS

/*materials = {
    {1.0f, 1.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.2f, 32.0f},
    {0.0f, 1.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.4f, 32.0f},
    {0.0f, 0.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {0.0f, 0.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.4f, 32.0f},
};*/ //PRESET MATERIALS

/*for (int i = 0; i < bound; i++) {
    int r = rand() % 1000, r2 = rand() % 1000;
    ObjectData o = { 0, 0, vec {r / 1000.0f, (float)i/5, r2 / 1000.0f}, 0.1, 1.0 };
    auto s = std::make_shared<Sphere>(o);
    objects.push_back(s);
}*/