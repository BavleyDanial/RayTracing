#include <string>
#include <memory>

#include <Renderer.h>
#include <Camera.h>
#include <Image.h>

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LOG(x, ...) printf(x, ##__VA_ARGS__) // Temporary, maybe I'll create a logging library for performance measurements

int main() {
    if (!glfwInit()) {
        LOG("Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "RayTracing", NULL, NULL);
    if (!window) {
        LOG("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Core::Scene scene;
    scene.materials.push_back({glm::vec3(0.0f, 0.0f, 1.0f)});
    scene.materials.push_back({glm::vec3(0.0f, 1.0f, 0.0f)});
    scene.directionalLights.push_back({ glm::vec3(-1.0f), glm::vec3(1.0f), 1.0f });

    {
        Core::Sphere sphere;
        scene.spheres.push_back(sphere);
    }
    {
        Core::Sphere sphere;
        sphere.position = glm::vec3(0.0f, 1.0f, 0.0f);
        sphere.materialIndex = 1;
        scene.spheres.push_back(sphere);
    }
    {
        Core::Sphere sphere;
        sphere.position = {-1.0f, 0.0f, 0.0f };
        scene.spheres.push_back(sphere);
    }

    glm::vec2 viewport(1);
    std::unique_ptr<Core::Image> image = std::make_unique<Core::Image>(1920, 1080, 4);
    Core::Camera camera(glm::vec3(0, 0, 3), viewport, 45.0f, 0.1f, 1000.0f);
    RT::Renderer renderer(scene);
    uint32_t frame = 1;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("RayTracing Viewport");
        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetContentRegionAvail().y;
        if (width != image->width || height != image->height) {
            image.reset(new Core::Image(width, height, 4));
            camera.OnResize({image->width, image->height});
        }

        float startTime = glfwGetTime();
        renderer.Render(camera, image.get(), frame++);
        float endTime = glfwGetTime();
        float deltaTime = (endTime - startTime) * 1000;

        ImGui::Image(texture, ImVec2(image->width, image->height), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Begin("RayTracing Options");
        ImGui::Text("DeltaTime: %f", deltaTime);
        ImGui::Text("Render Resolution: %ix%i", image->width, image->height);
        ImGui::Text("Spheres Count: %i", static_cast<int>(scene.spheres.size()));
        ImGui::Separator();
        ImGui::DragInt("Max Bounces", &renderer.bounceLimit, 1, 1);
        ImGui::DragInt("Rays Per Pixel", &renderer.raysPerPixel, 1, 1);
        ImGui::End();

        ImGui::Begin("Scene");
        if (ImGui::CollapsingHeader("Spheres")) {
            for (size_t i = 0; i < scene.spheres.size(); i++) {
                ImGui::PushID(("Sphere" + std::to_string(i)).c_str());
                ImGui::DragFloat3("Position", glm::value_ptr(scene.spheres[i].position), 0.1f);
                ImGui::DragFloat("Scale", &scene.spheres[i].radius, 0.1f);
                ImGui::InputInt("Material ID", &scene.spheres[i].materialIndex);
                if (ImGui::Button("Remove")) {
                    scene.spheres.erase(scene.spheres.begin() + i);
                }
                if (i != scene.spheres.size() - 1) {
                    ImGui::Separator();
                }
                ImGui::PopID();
            }
            ImGui::SameLine();
            ImGui::PushID("Sphere Add");
            if (ImGui::Button("Add")) {
                scene.spheres.push_back({});
            }
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Materials")) {
            for (size_t i = 0; i < scene.materials.size(); i++) {
                ImGui::PushID(("Material" + std::to_string(i)).c_str());
                ImGui::ColorEdit3("Albedo", glm::value_ptr(scene.materials[i].albedo));
                ImGui::ColorEdit3("Emission Color", glm::value_ptr(scene.materials[i].emissionColor));
                ImGui::DragFloat("Emission Strength", &scene.materials[i].emissionStrength);
                if (ImGui::Button("Remove")) {
                    scene.materials.erase(scene.materials.begin() + i);
                }
                if (i != scene.materials.size() - 1) {
                    ImGui::Separator();
                }
                ImGui::PopID();
            }
            ImGui::SameLine();
            ImGui::PushID("Material Add");
            if (ImGui::Button("Add")) {
                scene.materials.push_back({});
            }
            ImGui::PopID();
        }
        ImGui::End();

        ImGui::Render();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels.data());
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

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
