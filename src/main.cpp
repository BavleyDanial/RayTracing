#include <dinput.h>
#include <stdio.h>

#include <Renderer.h>
#include <Image.h>

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#define LOG(x, ...) printf(x, ##__VA_ARGS__) // Temporary, maybe I'll create a logging library for performance measurements

int main() {
    if (!glfwInit()) {
        LOG("Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Docking Example", NULL, NULL);
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
    scene.materials.push_back({glm::vec3(0.0f, 0.0f, 255.0f)});
    scene.directionalLights.push_back({ glm::vec3(-1.0f), glm::vec3(255.0f), 1.0f });

    {
        Core::Sphere sphere;
        scene.spheres.push_back(sphere);
    }
    {
        Core::Sphere sphere;
        sphere.position = {-1.0f, 0.0f, 0.0f };
        scene.spheres.push_back(sphere);
    }
    
    Core::Camera camera(glm::vec3(0, 0, 1), glm::vec2(0));
    RT::Renderer renderer(scene);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("RayTracing Viewport");
        Core::Image image(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, 4);
        camera.SetViewport({ 2*image.aspectRatio, 2.0f });

        float startTime = glfwGetTime();
        renderer.Render(camera, image);
        float endTime = glfwGetTime();
        float deltaTime = (endTime - startTime) * 1000;
        
        ImGui::Image(texture, ImVec2(image.width, image.height), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        
        ImGui::Begin("RayTracing Options");
        ImGui::Text("DeltaTime: %f", deltaTime);
        ImGui::End();

        ImGui::Render();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
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