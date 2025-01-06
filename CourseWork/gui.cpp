#include "gui.hpp"

#include <iostream>

using namespace ImGui;

GUI::GUI(int width, int height) :
    width {width}, height {height} {}

void GUI::mainMenu(int& screen) {
    // Main Menu
}

void GUI::clampFloat(float& f, float min, float max) {
    if (f < min) f = min;
    if (f > max) f = max;
}

void GUI::screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, std::vector<Material>& materials, float fps, float &g, std::vector<std::string> tArr, std::vector<std::string> tMatArr) {
    static bool debugFlag = false, addFlag = false, materialsFlag = false, newMaterialFlag = false;
    if (BeginMainMenuBar()) {
        if (BeginMenu("File")) {                                //TODO: FILE MENU
            if (MenuItem("Open", "Ctrl+O")) {
                // Handle Open Action
            }
            if (MenuItem("Save", "Ctrl+S")) {
                // Handle Save Action
            }
            if (MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            EndMenu();
        }

        if (BeginMenu("Edit")) {
            if (MenuItem("Undo", "Ctrl+Z")) {
                // Handle Undo Action
            }
            if (MenuItem("Redo", "Ctrl+Y")) {
                // Handle Redo Action
            }
            if (MenuItem("Add")) {
                addFlag = true;
            }
            if (MenuItem("Materials")) {
                materialsFlag = true;
            }
            EndMenu();
        }

        // Settings Menu
        if (BeginMenu("Settings")) {
            if (MenuItem("Graphics")) {
                // Handle Graphics Settings
            }
            if (MenuItem("Controls")) {
                // Handle Control Settings
            }
            if (MenuItem("Debug")) {
                debugFlag = true;
            }
            EndMenu();
        }

        EndMainMenuBar();
    }

    // Rendering other GUI elements or application-specific logic
    if (debugFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Debug", &debugFlag)) {
            PushTextWrapPos();
            Text("FPS: %.2f", fps);
            Text("Gravity: %.2f", g);
            for (const auto& text : tArr) {
                BulletText("%s", text.c_str());
            }
        }
        PopTextWrapPos();
        End();
    }
    if (addFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Add", &addFlag)) {
            PushTextWrapPos();
            static float pos[3], radius, mass;
            static int material;
            Text("Add a sphere to the scene");
            clampFloat(pos[0], -2.0, 2.0);
            clampFloat(pos[1], -2.0, 2.0);
            clampFloat(pos[2], -2.0, 2.0);
            Text("Data:");
            InputFloat3("Position", pos);
            InputFloat("Radius", &radius);
            clampFloat(radius, 0.1, 1.0);
            InputFloat("Mass", &mass);
            clampFloat(mass, 0.1, 50.0);
            SliderInt("Material", &material, 1, materials.size()-1);  // Offset for user experience
            if (Button("Create") && material != 0) {
                int r[2] = { rand() % 1000, rand() % 1000 };
                ObjectData o = { 0, material, vec {pos[0] + r[0] / 100000.0f, pos[1], pos[2] + r[1] / 100000.0f}, radius, mass };
                auto s = std::make_shared<Sphere>(o);
                objects.push_back(s);
            }
        }
        PopTextWrapPos();
        End();
    }
    if (materialsFlag) {
        SetNextWindowSize(ImVec2(400, 600), ImGuiCond_Appearing);
        if (Begin("Materials", &materialsFlag)) {
            PushTextWrapPos();
            int i = 0, j = 1;
            for (const auto text : tMatArr) {
                if (i >= 7) {
                    BulletText("%s", text.c_str());
                    if (i % 7 == 1) {
                        ImVec4 colorPreview(materials.at(j).r, materials.at(j).g, materials.at(j).b, 1.0f);
                        SameLine();
                        ColorButton("Color Preview", colorPreview);
                        j++;
                    }
                }
                i++;
            }
            if (Button("Add new")) {
                newMaterialFlag = true;
            }
            PopTextWrapPos();
        }
        End();
        if (newMaterialFlag) {
            SetNextWindowSize(ImVec2(400, 600), ImGuiCond_Appearing);
            if (Begin("New Material", &newMaterialFlag)) {
                PushTextWrapPos();
                static float col[3], Ka, Kd, Ks, Kr, c;
                ColorPicker3("Colour", col);
                SliderFloat("Ambient", &Ka, 0.0, 1.0);
                SliderFloat("Diffuse", &Kd, 0.0, 1.0);
                SliderFloat("Specular", &Ks, 0.0, 1.0);
                SliderFloat("Reflectiveness", &Kr, 0.0, 1.0);
                SliderFloat("Shininess", &c, 1.0, 256.0);
                if (Button("Add")) {
                    Material m = { col[0], col[1], col[2], Ka, Kd, Ks, Kr, c };
                    materials.push_back(m);
                }
                PopTextWrapPos();
            }
            End();
        }
    }
}
