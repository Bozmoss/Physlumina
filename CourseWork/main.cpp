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
int frameCount = 0;
double prevTime = 0.0;
float g = 4, r = 0.7, f = 0.5, fps = 0.0f;
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
    if (key == GLFW_KEY_J && action == GLFW_PRESS) {
        for (auto& o : objects) {
            int r[3] = { (rand() % 201) - 100, rand() % 100, (rand() % 201) - 100 };
            o->getData()->vel = vec{ r[0] / 100.0f, r[1] / 40.0f, r[2] / 40.0f };
        }
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

void update(float eTime, float dt) {
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - prevTime >= 1.0) {
        fps = frameCount;
        frameCount = 0;
        prevTime = currentTime;
    }
    text.clear();
    matText.clear();
    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](const std::shared_ptr<Object>& o) {
            return abs(o->getData()->r.x) >= 8.5 || abs(o->getData()->r.z) >= 8.5;
        }),
        objects.end()
    );
    for (auto& o : objects) {
        o->updateObject(g, r, eTime, dt);
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

void updateObjectDatas(float dt) {
    for (auto& o : objects) {
        o->update(f, dt);
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

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGl", glfwGetPrimaryMonitor(), NULL);
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
        {0.0, 2.0, 1.0}
    };

    std::vector<std::vector<float>> lightCols = { // DEFAULT
        {1.0, 1.0, 1.0},
    };

    materials = {
    {0.0f, 1.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.0f, 0.0f}, // FLOOR
    {1.0f, 0.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {0.0f, 1.0f, 0.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {0.0f, 0.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.0f, 16.0f},
    {1.0f, 1.0f, 1.0f, 0.2f, 0.7f, 0.5f, 0.2f, 32.0f}
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

    double lastFrameTime = glfwGetTime(), initalTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        p.activate();

        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        update(currentFrameTime - initalTime, deltaTime);

        updateObjectDatas(deltaTime);
        
        fvs.update(p, game.getAX(), game.getAY(), materials, objects, lights, lightCols);
        
        glDrawElements(GL_TRIANGLES, iB.number(), GL_UNSIGNED_INT, nullptr);

        gui.screenOne(window, objects, materials, r, f, g, fps, text, matText, lights, lightCols);
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