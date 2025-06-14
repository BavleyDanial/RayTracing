#include <string>
#include <memory>

#include <Renderer.h>
#include <Camera.h>
#include <Image.h>
#include <ImageFile.h>

#include <glad/glad.h>
#include <imgui.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture2D.h>

#define LOG(x, ...) printf(x, ##__VA_ARGS__) // Temporary, maybe I'll create a logging library for performance measurements

#define IMGUI_UNLIMITED_FRAME_RATE

int main() {
    if (!glfwInit()) {
        LOG("Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "RayTracing", nullptr, nullptr);
    if (!window) {
        LOG("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG("Failed to load GLAD\n");
        glfwTerminate();
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    GLuint renderTexture;
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    RT::Shader RTShader = RT::Shader("resources/shaders/raytracing.glsl");

    Core::Scene scene;
    scene.materials.push_back({glm::vec3(124.0f/255.0f, 252.0f/255.0f, 0.0f)});
    scene.materials.push_back({glm::vec3(1.0f, 46.0f/255.0f, 0.0f), glm::vec3(0.0f, 191.0f/255.0f, 21.0f/255.0f), 20.0f});
    scene.materials.push_back({glm::vec3(204.0f/255.0f, 128.0f/255.0f, 51.0f/255.0f)});

    {
        Core::Sphere sphere;
        sphere.position = glm::vec3(0.0f, -100.5f, 0.0f);
        sphere.radius = 100.0f;
        sphere.materialIndex = 0;
        scene.spheres.push_back(sphere);
    }
    {
        Core::Sphere sphere;
        sphere.position = glm::vec3(33.0f, 4.0f, -32.0f);
        sphere.radius = 20.0f;
        sphere.materialIndex = 1;
        scene.spheres.push_back(sphere);
    }
    {
        Core::Sphere sphere;
        sphere.materialIndex = 2;
        scene.spheres.push_back(sphere);
    }

    glm::vec2 viewport(1);
    std::unique_ptr<Core::Image> image = std::make_unique<Core::Image>(1920, 1080, 4);
    Core::Camera camera(glm::vec3(0, 0, 3), viewport, 45.0f, 0.1f, 1000.0f);
    RT::Renderer renderer(scene);

    uint32_t frame = 1;
    bool accumulate = false;
    uint32_t framesAccToSave = 1000;
    uint32_t pngImageCount = 0;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("RayTracing Viewport");
        uint32_t width = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        uint32_t height = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);
        if (width != image->width || height != image->height) {
            image.reset(new Core::Image(width, height, 4));
            camera.OnResize({image->width, image->height});
            renderer.OnResize(image->width, image->height);
            frame = 1;
        }

        float startTime = static_cast<float>(glfwGetTime());
        renderer.Render(camera, image.get(), frame);
        if (accumulate)
            frame++;
        if (frame == framesAccToSave) {
            Core::ImagePNG png(image.get());
            png.Save("resources/out/output" + std::to_string(pngImageCount));
        }

        float endTime = static_cast<float>(glfwGetTime());
        float deltaTime = (endTime - startTime) * 1000;
        int frameRate = static_cast<int>(1.0f / (endTime - startTime));

        ImGui::Image(renderTexture, ImVec2(static_cast<float>(image->width), static_cast<float>(image->height)), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Begin("RayTracing Options");
        
        ImGui::Text("Delta Time: %f", deltaTime);
        ImGui::Text("Frame Rate: %i", frameRate);
        ImGui::Text("Render Resolution: %ix%i", static_cast<int>(image->width), static_cast<int>(image->height));
        ImGui::Text("Spheres Count: %i", static_cast<int>(scene.spheres.size()));
        ImGui::Text("Frames Accumulated: %i", static_cast<int>(frame));
        ImGui::Text("Frames Accumulated to Save: %i", static_cast<int>(framesAccToSave));
        
        ImGui::Separator();
        
        ImGui::SliderInt("Max Bounces", &renderer.bounceLimit, 1, 8);
        ImGui::DragFloat("Gamma Correction", &renderer.gamma, 0.1f);
        ImGui::DragFloat("Exposure", &renderer.exposure, 0.1f);
        
        if (ImGui::Checkbox("Apply Gamma Correction", &renderer.doGammaCorrection))
            frame = 1;
        if (ImGui::Checkbox("Apply ToneMapping", &renderer.doToneMapping))
            frame = 1;
        if (ImGui::Checkbox("Accumulate", &accumulate))
            frame = 1;
        if (ImGui::Button("Reset Accumulated Data"))
            frame = 1;
        
        ImGui::End();

        ImGui::Begin("Scene");
        if (ImGui::CollapsingHeader("SkyLight")) {
            ImGui::ColorEdit3("Albedo", glm::value_ptr(scene.skyLight.color));
            ImGui::DragFloat("Strength", &scene.skyLight.strength, 0.1f);
        }
        if (ImGui::CollapsingHeader("Spheres")) {
            for (size_t i = 0; i < scene.spheres.size(); i++) {
                ImGui::PushID(("Sphere" + std::to_string(i)).c_str());
                ImGui::DragFloat3("Position", glm::value_ptr(scene.spheres[i].position), 0.1f);
                ImGui::DragFloat("Scale", &scene.spheres[i].radius, 0.1f);
                ImGui::InputInt("Material ID", &scene.spheres[i].materialIndex);
                if (ImGui::Button("Remove")) {
                    scene.spheres.erase(scene.spheres.begin() + static_cast<uint32_t>(i));
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
                ImGui::DragFloat("Emission Strength", &scene.materials[i].emissionStrength, 1);
                ImGui::SliderFloat("Shininess", &scene.materials[i].shininess, 0, 1);
                if (ImGui::Button("Remove")) {
                    scene.materials.erase(scene.materials.begin() + static_cast<uint32_t>(i));
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(image->width), static_cast<int>(image->height), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels.data());
        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
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
