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
#include <string>

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

std::vector<Material> materials;
std::vector<std::shared_ptr<Object>> objects;
Game game(objects);
int screen = 0, frameCount = 0;
double prevTime = 0.0;
float g = 0.0005, r = 0.7, f = 0.5, fps = 0.0f;
const int bound = 10;
std::vector<std::string> text, matText;

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
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        int r[2] = { rand() % 100, rand() % 100 };
        ObjectData o = { 0, 1, vec {r[0] / 100.0f, 1.5, r[1] / 100.0f}, 0.5, 1.0 };
        auto s = std::make_shared<Sphere>(o);
        objects.push_back(s);
    }
}

void printObjectData(std::shared_ptr<Object>& o) {
    text.push_back("----------OBJECT----------");
    text.push_back("Type:             " + std::to_string(o->getData()->type));
    text.push_back("Material:         " + std::to_string(o->getData()->material));
    text.push_back("Position:         " + std::to_string(o->getData()->r.x) + " " + std::to_string(o->getData()->r.y) + " " + std::to_string(o->getData()->r.z));
    text.push_back("Defining length:  " + std::to_string(o->getData()->l1));
    text.push_back("Mass:             " + std::to_string(o->getData()->mass));
    text.push_back("Velocity:         " + std::to_string(o->getData()->vel.x) + " " + std::to_string(o->getData()->vel.y) + " " + std::to_string(o->getData()->vel.z));
    text.push_back("Down:             " + std::to_string(o->getData()->down));
    text.push_back("Floor:            " + std::to_string(o->getData()->floor));
}

void printMaterialData(Material& m) {
    matText.push_back("----------MATERIAL----------");
    matText.push_back("RGB Value:      " + std::to_string(m.r) + " " + std::to_string(m.g) + " " + std::to_string(m.b));
    matText.push_back("Ambient:        " + std::to_string(m.Ka));
    matText.push_back("Diffuse:        " + std::to_string(m.Kd));
    matText.push_back("Specular:       " + std::to_string(m.Ks));
    matText.push_back("Reflectiveness: " + std::to_string(m.Kr));
    matText.push_back("Shininess:      " + std::to_string(m.c));
}

void update() {
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - prevTime >= 1.0) {
        fps = frameCount;
        frameCount = 0;
        prevTime = currentTime;
    }
    text.clear();
    matText.clear();
    for (auto& o : objects) {
        o->updateObject(g, r);
        printObjectData(o);
    }
    for (auto& m : materials) {
        printMaterialData(m);
    }
    int i = 0;
    for (auto& o : objects) {
        for (int j = i + 1; j < objects.size(); j++) {
            if (o->checkCollision((*objects.at(j)))) {
                o->resolveCollision((*objects.at(j)));
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
    io.ConfigDebugHighlightIdConflicts = false;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440");

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 10.0f;
    style.PopupRounding = 10.0f;
    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 10.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);

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

    std::vector<std::vector<float>> lightCols = { // DEFAULT
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0}
    };

    materials = {
    {0.0f, 1.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.0f, 0.0f}, // FLOOR
    {1.0f, 0.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {0.0f, 1.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {0.0f, 0.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {1.0f, 1.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.2f, 32.0f}
    };

    int r[4] = { rand() % 100, rand() % 100, rand() % 100, rand() % 100 };

    ObjectData objectDatas[] = {
        {0, 2, vec {r[0] / 100.0f, 1.5, r[1] / 100.0f}, 0.3, 1.0},
        {0, 2, vec {r[2] / 100.0f, 0.5, r[3] / 100.0f}, 0.4, 1.0}
    };

    for (int i = 0; i < sizeof(objectDatas) / sizeof(objectDatas[0]); i++) {
        switch (objectDatas[i].type) {
        case 0:
            auto s = std::make_shared<Sphere>(objectDatas[i]);
            objects.push_back(s);
            break;
        }
    }

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

        p.activate();
        update();
        updateObjectDatas();
        
        fvs.update(p, game.getAX(), game.getAY(), materials, objects);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDrawElements(GL_TRIANGLES, iB.number(), GL_UNSIGNED_INT, nullptr);

        gui.screenOne(window, objects, materials, fps, g, text, matText);
        ImGui::Render();
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


//while (!glfwWindowShouldClose(window)) {
//    glfwPollEvents();
//
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//    switch (screen) {
//    case 0:
//        gui.mainMenu(screen);
//        break;
//    case 1:
//        p.activate();
//        update();
//        updateObjectDatas();
//
//        fvs.update(p, game.getAX(), game.getAY(), materials, objects);
//
//        // Clear the screen
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glDrawElements(GL_TRIANGLES, iB.number(), GL_UNSIGNED_INT, nullptr);
//
//        gui.screenOne(window, objects, fps, g, text);
//        break;
//    }
//
//    ImGui::Render();
//    int display_w, display_h;
//    glfwGetFramebufferSize(window, &display_w, &display_h);
//    glViewport(0, 0, display_w, display_h);
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//    glfwSwapBuffers(window);
//}
//
//ImGui_ImplOpenGL3_Shutdown();
//ImGui_ImplGlfw_Shutdown();
//ImGui::DestroyContext();
//glfwDestroyWindow(window);
//glfwTerminate();
//return 0;