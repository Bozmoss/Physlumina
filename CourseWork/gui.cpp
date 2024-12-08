#include "gui.hpp"

using namespace ImGui;

GUI::GUI(int width, int height) :
    width {width}, height {height} {}

void GUI::mainMenu(int& screen) {
    // Main Menu
}

void GUI::screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, float fps, float &g, std::vector<std::string> tArr) {
    Begin("Sphere");
    Text("Add a sphere to the scene");
    if (Button("Add")) {
        int r[2] = { rand() % 100, rand() % 100 };
        ObjectData o = { 0, 1, vec {r[0] / 100.0f, 1.5, r[1] / 100.0f}, 0.5, 1.0 };
        auto s = std::make_shared<Sphere>(o);
        objects.push_back(s);
    }
    End();
}
