#include "gui.hpp"

#include <iostream>

using namespace ImGui;
namespace fs = std::filesystem;

GUI::GUI(int width, int height) :
    width {width}, height {height} {}

void GUI::clampFloat(float& f, float min, float max) {
    if (f < min) f = min;
    if (f > max) f = max;
}

std::string GUI::serializeMaterials(const std::vector<Material>& materials) {
    std::string result = "Materials:\n";
    for (const auto& m : materials) {
        result += "r:" + std::to_string(m.r) + " g:" + std::to_string(m.g) + " b:" + std::to_string(m.b) + " ";
        result += "Ka:" + std::to_string(m.Ka) + " Kd:" + std::to_string(m.Kd) + " Ks:" + std::to_string(m.Ks) + " ";
        result += "Kr:" + std::to_string(m.Kr) + " c:" + std::to_string(m.c) + "\n";
    }
    return result;
}

std::string GUI::serializeObjects(const std::vector<std::shared_ptr<Object>>& objects) {
    std::string result = "Objects:\n";
    for (const auto& obj : objects) {
        auto data = obj->getData();
        result += "type:" + std::to_string(data->type) + " material:" + std::to_string(data->material) + " ";
        result += "pos:" + std::to_string(data->r.x) + "," + std::to_string(data->r.y) + "," + std::to_string(data->r.z) + " ";
        result += "radius:" + std::to_string(data->l1) + " mass:" + std::to_string(data->mass) + "\n";
    }
    return result;
}

std::string GUI::serializeScene(const std::vector<Material>& materials, const std::vector<std::shared_ptr<Object>>& objects) {
    std::string sceneData = serializeMaterials(materials);
    sceneData += serializeObjects(objects);
    return sceneData;
}

void GUI::deserialize(std::string data, std::vector<Material>& materials, std::vector<std::shared_ptr<Object>>& objects) {
    std::istringstream stream(data);
    std::string line;
    bool parsingMaterials = false, parsingObjects = false;
    while (std::getline(stream, line)) {
        if (line.find("Materials:") == 0) {
            parsingMaterials = true;
            continue;
        }
        if (line.find("Objects:") == 0) {
            parsingMaterials = false;
            parsingObjects = true;
            continue;
        }
        if (parsingMaterials) {
            if (line.empty()) {
                continue;
            }

            std::regex materialRegex("r:([0-9\\.]+) g:([0-9\\.]+) b:([0-9\\.]+) Ka:([0-9\\.]+) Kd:([0-9\\.]+) Ks:([0-9\\.]+) Kr:([0-9\\.]+) c:([0-9\\.]+)");
            std::smatch match;
            if (std::regex_search(line, match, materialRegex)) {
                Material m;
                m.r = std::stof(match[1].str());
                m.g = std::stof(match[2].str());
                m.b = std::stof(match[3].str());
                m.Ka = std::stof(match[4].str());
                m.Kd = std::stof(match[5].str());
                m.Ks = std::stof(match[6].str());
                m.Kr = std::stof(match[7].str());
                m.c = std::stof(match[8].str());
                materials.push_back(m);
            }
        }
        if (parsingObjects) {
            if (line.empty()) {
                continue;
            }
            std::regex objectRegex("type:([0-9]+) material:([0-9]+) pos:(\\-?[0-9\\.]+),(\\-?[0-9\\.]+),(\\-?[0-9\\.]+) radius:([0-9\\.]+) mass:([0-9\\.]+)");
            std::smatch match;
            if (std::regex_search(line, match, objectRegex)) {
                int type = std::stoi(match[1].str());
                int material = std::stoi(match[2].str());
                float posX = std::stof(match[3].str());
                float posY = std::stof(match[4].str());
                float posZ = std::stof(match[5].str());
                float radius = std::stof(match[6].str());
                float mass = std::stof(match[7].str());

                ObjectData objectData = { type, material, vec{posX, posY, posZ}, radius, mass };
                std::shared_ptr<Object> obj = std::make_shared<Sphere>(objectData);
                objects.push_back(obj);
            }
        }
    }
}

void GUI::screenOne(GLFWwindow* window, std::vector<std::shared_ptr<Object>>& objects, std::vector<Material>& materials, float &r, float &f, float &g, float fps, std::vector<std::string> tArr, std::vector<std::string> tMatArr, std::vector<std::vector<float>> lights, std::vector<std::vector<float>> lightCols) {
    static bool debugFlag = false, addFlag = false, materialsFlag = false, newMaterialFlag = false, controlsFlag = false, saveFlag = false, loadFlag = false, constantsFlag = false, lightsFlag = false, newLightFlag = false;
    static std::vector<std::string> files;
    static int index = -1;
    static float G = 9.81;
    if (BeginMainMenuBar()) {
        if (BeginMenu("File")) {
            if (MenuItem("Open")) {
                loadFlag = true;
                if (files.empty()) {
                    for (const auto& entry : fs::directory_iterator(".")) {
                        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                            std::string s = entry.path().filename().string();
                            files.push_back(s);
                        }
                    }
                }
            }
            if (MenuItem("Save")) {
                saveFlag = true;
            }
            if (MenuItem("Exit", "Esc")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            EndMenu();
        }

        if (BeginMenu("Edit")) {
            if (MenuItem("Objects")) {
                addFlag = true;
            }
            if (MenuItem("Materials")) {
                materialsFlag = true;
            }
            if (MenuItem("Lights")) {
                lightsFlag = true;
            }
            EndMenu();
        }

        if (BeginMenu("Settings")) {
            if (MenuItem("Controls")) {
                controlsFlag = true;
            }
            if (MenuItem("Debug")) {
                debugFlag = true;
            }
            if (MenuItem("Constants")) {
                constantsFlag = true;
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
        if (Begin("Objects", &addFlag)) {
            PushTextWrapPos();
            static float pos[3], radius, mass;
            static int material = 1;
            Text("Add a sphere to the scene");
            clampFloat(pos[0], -10.0, 10.0);
            clampFloat(pos[1], -10.0, 10.0);
            clampFloat(pos[2], -10.0, 10.0);
            Text("Data:");
            InputFloat3("Position", pos);
            InputFloat("Radius", &radius);
            clampFloat(radius, 0.1, 1.0);
            InputFloat("Mass", &mass);
            clampFloat(mass, 0.1, 50.0);
            SliderInt("Material", &material, 1, materials.size()-1); // Offset for user experience
            if (Button("Create") && material != 0) {
                int r[2] = { rand() % 1000, rand() % 1000 };
                ObjectData o = { 0, material, vec {pos[0] + r[0] / 100000.0f, pos[1], pos[2] + r[1] / 100000.0f}, radius, mass };
                auto s = std::make_shared<Sphere>(o);
                objects.push_back(s);
            }
            if (Button("Remove last") && objects.size() > 0) {
                objects.pop_back();
            }
            if (Button("Clear all")) {
                objects.clear();
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
    if (controlsFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Controls", &controlsFlag)) {
            PushTextWrapPos();
            BulletText("Click and drag to look around");
            BulletText("Press Esc to quit");
            BulletText("Add objects or materials from the Edit menu");
            BulletText("Press J to make the objects jump");
            PopTextWrapPos();
        }
        End();
    }
    if (saveFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Save", &saveFlag)) {
            PushTextWrapPos();
            static char buf[255] = "";
            InputText("File name", buf, sizeof(buf));
            if (Button("Save")) {
                std::string fName = std::string(buf) + ".txt";
                std::string data = serializeScene(materials, objects);
                FileHandler f(fName.c_str());
                f.writeFile(data);
                saveFlag = false;
            }
            PopTextWrapPos();
        }
        End();
    }
    if (loadFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Load")) {
            PushTextWrapPos();
            if (!files.empty()) {
                Combo("Select file", &index, files.data()->c_str(), files.size());
                if (Button("Load") && index != -1) {
                    FileHandler f(files[index].c_str());
                    std::string data = f.readFile();
                    if (!data.empty()) {
                        deserialize(data, materials, objects);
                    }
                    loadFlag = false;
                }
            }
            PopTextWrapPos();
        }
        End();
    }
    if (constantsFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Constants"), &constantsFlag) {
            PushTextWrapPos();
            InputFloat("Gravity", &G);
            clampFloat(G, 0.0, 30.0);
            g = G * (0.0005 / 9.81);
            InputFloat("Restitution", &r);
            clampFloat(r, 0.0, 1.0);
            InputFloat("Friction", &f);
            clampFloat(f, 0.0, 1.0);
            PopTextWrapPos();
        }
        End();
    }
    if (lightsFlag) {
        SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
        if (Begin("Lights"), &lightsFlag) {
            PushTextWrapPos();
            for (int i = 0; i < lights.size(); i++) {
                std::string s1 = "Position: " + std::to_string(lights.at(i).at(0)) + " " + std::to_string(lights.at(i).at(1)) + " " + std::to_string(lights.at(i).at(2));
                std::string s2 = "Colour:   " + std::to_string(lightCols.at(i).at(0)) + " " + std::to_string(lightCols.at(i).at(1)) + " " + std::to_string(lightCols.at(i).at(2));
                ImVec4 colorPreview(lightCols.at(i).at(0), lightCols.at(i).at(1), lightCols.at(i).at(2), 1.0f);
                BulletText(s1.c_str());
                BulletText(s2.c_str());
                SameLine();
                ColorButton("Color Preview", colorPreview);
            }
            if (Button("Add new")) {
                newLightFlag = true;
            }
            PopTextWrapPos();
        }
        End();
        if (newLightFlag) {
            SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
            if (Begin("New Light"), &newLightFlag) {
                PushTextWrapPos();
                static float col[3], pos[3];
                ColorPicker3("Colour", col);
                InputFloat3("Position", pos);
                clampFloat(pos[0], -5.0, 5.0);
                clampFloat(pos[1], 0.6, 5.0);
                clampFloat(pos[2], -5.0, 5.0);
                if (Button("Add")) {
                    lights.push_back({ pos[0], pos[1], pos[2] });
                    lightCols.push_back({ col[0], col[1], col[2] });
                }
                PopTextWrapPos();
            }
            End();
        }
    }
}
