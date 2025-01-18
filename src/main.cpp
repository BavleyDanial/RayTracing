#include <stdio.h>

#include <Image.h>
#include <ImageFile.h>

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#define LOG(x, ...) printf(x, ##__VA_ARGS__) // Temporary, maybe I'll create a logging library for performance measurements

struct Ray {
    glm::vec3 org;
    glm::vec3 dir;
};

// TODO: when a ray misses, try using an environment map (cube map) instead of computing colors
glm::vec3 RayMiss() {
    return glm::vec3(0.0f);
}

/*
 *  (Cx - x)^2 + (Cy - y)^2 + (Cz - z)^2 = r^2, where C is the Circle position and r is the radius
 *  From This equation, x, y, z, are the components of the ray, which results in the following
 *  (Cx - (Ox + tDx))^2 + (Cy - (Oy + tDy))^2 + (Cz - (Oz + tDz))^2 = r^2 where O is the origin ray, D is the direction and t is scalar to solve for
 *  For simplicity we can assume the circle is at the origin of the world, so the result is the following
 *  (Ox + tDx) ^ 2 + (Oy + tDy) ^ 2 + (Oz + tDz)^2 = r^2
 *  Ox^2 + 2OxtDx + t^2Dx^2 + Oy^2 + 2OytDy + t^2Dy^2 + Oz^2 + 2OztDz + t^2Dz^2 = r^2
 *  t^2(Dx^2 + Dy^2 + Dz^2) + 2t(OxDx + OyDy + OzDz) + (Ox^2 + Oy^2 + Oz^2) = r^2
 *
 *  t^2 (D.D) + 2t(O.D) + (O.O) - r^2 = 0
 *  Solve for t using quadratic formula
*/
glm::vec3 TraceRay(const Ray& ray) {
    glm::vec3 lightDir(-1.0f);
    float lightIntensity = 0.5f;

    glm::vec3 circlePos(0.0f);
    float radius = 0.5f;

    glm::vec3 co = ray.org - circlePos; // if the camera is moved somewhere offset the rendering as if the circle is at the origin of the camera
    float a = glm::dot(ray.dir, ray.dir);
    float b = 2.0f * glm::dot(co, ray.dir);
    float c = glm::dot(co, co) - radius*radius;
    float discriminant = b*b - 4*a*c;

    // (-b +- sqrt(b^2 - 4ac))/2a
    if (discriminant < 0) {
        return RayMiss();
    }

    float t0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

    glm::vec3 hit0 = ray.org + t0 * ray.dir;
    glm::vec3 hit1 = ray.org + t1 * ray.dir;

    // Diffuse Lighting
    glm::vec3 hitNorm = glm::normalize(hit0 - circlePos);
    float cosTerm = glm::dot(hitNorm, -lightDir);

    return lightIntensity * cosTerm * glm::vec3(0, 0, 255);
}

void Render(RT::Image& image) {
    glm::vec2 viewport(2*image.aspectRatio, 2.0f);
    glm::vec3 cameraPos(0, 0, 1);

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            glm::vec2 fragPos = { (float)x / (image.width - 1), (float)y / (image.height - 1) };
            fragPos = fragPos * 2.0f - 1.0f;
            fragPos *= viewport;

            glm::vec3 rayDir = glm::vec3(fragPos.x, fragPos.y, -1.0f);
            Ray ray(cameraPos, rayDir);

            glm::vec3 pixelColor = TraceRay(ray);
            pixelColor = glm::clamp(pixelColor, glm::vec3(0), glm::vec3(255));

            int pixelIdx = image.comps * (y * image.width + x);
            DrawPixel(image, pixelIdx, {pixelColor, 255});
        }
    }
}

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

    float deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("RayTracing Viewport");
        RT::Image image(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, 4);
        
        float startTime = glfwGetTime();
        Render(image);
        float endTime = glfwGetTime();
        deltaTime = (endTime - startTime) * 1000;
        
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