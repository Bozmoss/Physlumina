#include "gui.hpp"

using namespace ImGui;

GUI::GUI(int width, int height) :
    width {width}, height {height} {}

void GUI::mainMenu(int& screen) {
    SetNextWindowPos(ImVec2(width / 2 - width / 10.0, height / 2 - height / 10.0));
    SetNextWindowSize(ImVec2(width / 5.0, height / 5.0));
    Begin("Main Menu");
    Text("Select one of the options to either make a new file, \nor load a file");

    if (Button("New")) {
        screen++;
    }
    SameLine();
    if (Button("Load")) {
        // Handle Load action
    }
    End();
}

void GUI::screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, float fps, float &g, std::vector<std::string> tArr) {
    static bool sphereWindow = false, FPS = false, constants = false, debug = false;
    if (BeginMainMenuBar()) {
        if (BeginMenu("Objects")) {
            if (MenuItem("Sphere")) {
                sphereWindow = true;
            }
            EndMenu();
        }
        if (BeginMenu("Global values")) {
            if (MenuItem("Constants")) {
                constants = true;
            }
            EndMenu();
        }
        if (BeginMenu("View")) {
            if (MenuItem("FPS")) {
                FPS = true;
            }
            EndMenu();
        }
        if (BeginMenu("Debug")) {
            if (MenuItem("Open debug menu")) {
                debug = true;
            }
            EndMenu();
        }
        EndMainMenuBar();
    }
    if (sphereWindow) {
        SetNextWindowSize(ImVec2(width / 5.0, height / 5.0));
        static int m = 0;
        static float x = 0.0, y = 0.0, z = 0.0, r = 0.3, mass = 1.0;
        if (Begin("Sphere", &sphereWindow)) {
            SliderInt("Material", &m, 0, 4);
            SliderFloat("x", &x, -1, 1);
            SliderFloat("y", &y, -1, 1);
            SliderFloat("z", &z, -1, 1);
            SliderFloat("Radius", &r, 0.1, 0.8);
            SliderFloat("Mass", &mass, 0.1, 2);
            if (Button("Create")) {
                ObjectData o = { 0, m, vec {x,y,z}, r, mass };
                auto s = std::make_shared<Sphere>(o);
                objects.push_back(s);
            }
        }
        End();
    }
    if (FPS) {
        SetNextWindowSize(ImVec2(width / 5.0, height / 5.0));
        if (Begin("FPS", &FPS)) {
            Text("Current FPS: %.1f", fps);
        }
        End();
    }
    if (constants) {
        SetNextWindowSize(ImVec2(width / 5.0, height / 5.0));
        static float G = 50.0;
        if (Begin("Constants", &constants)) {
            SliderFloat("Gravity", &G, 50.0, 1000.0);
            g = G/1000000;
        }
        End();
    }
    if (debug) {
        SetNextWindowSize(ImVec2(width / 5.0, height / 5.0));
        if (Begin("Debug", &debug)) {
            for (std::string t : tArr) {
                Text(t.c_str());
                Text("\n");
            }
        }
        End();
    }
}
