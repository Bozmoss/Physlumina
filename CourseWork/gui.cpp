#include "gui.hpp"

using namespace ImGui;

GUI::GUI(int width, int height) :
    width {width}, height {height} {}

void GUI::mainMenu(int& screen) {
    // Main Menu
}

void GUI::screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, float fps, float &g, std::vector<std::string> tArr) {
    static bool debugFlag = false;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 10.0f;
    style.PopupRounding = 10.0f;
    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 10.0f;

    // Optional: Adjust style colors if desired
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);

    if (BeginMainMenuBar()) {
        if (BeginMenu("File")) {
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
            if (MenuItem("Preferences")) {
                // Handle Preferences Action
            }
            EndMenu();
        }

        // Settings Menu
        if (BeginMenu("Settings")) {
            if (MenuItem("Graphics")) {
                // Handle Graphics Settings
            }
            if (MenuItem("Audio")) {
                // Handle Audio Settings
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
        Text("FPS: %.2f", fps);
        Text("Gravity: %.2f", g);
        for (const auto& text : tArr) {
            BulletText("%s", text.c_str());
        }
    }
}
