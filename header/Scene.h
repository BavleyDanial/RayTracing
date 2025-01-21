#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Core {

    struct DirectionalLight {
        glm::vec3 direction{-1};
        glm::vec3 color{1.0f};
        float intensity = 1.0f;
    };

    struct PointLight {
        glm::vec3 Position{0};
        glm::vec3 color{0};
        float intensity = 1.0f;
    };

    struct Material {
        glm::vec3 albedo{1.0f, 0, 1.0f};
    };

    struct Sphere {
        // Sphere data
        glm::vec3 position{0};
        float radius = 0.5f;

        // Indices to other structs
        int materialIndex = 0;
    };

    // TODO: Possibly create memory arenas to help performance of retrieving 
    struct Scene {
        std::vector<DirectionalLight> directionalLights;
        std::vector<PointLight> pointLights;
        std::vector<Material> materials;    // First material is always pink so that any object that doesn't have a material has a default value
        std::vector<Sphere> spheres;
    };

}

