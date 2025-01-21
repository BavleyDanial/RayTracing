#pragma once

#include <Scene.h>

#include <Image.h>
#include <Camera.h>

namespace RT {

    // TODO: Maybe change to be a private struct in Renderer
    struct Ray {
        glm::vec3 org;
        glm::vec3 dir;
    };
    
    class Renderer {
    public:
        Renderer(const Core::Scene& scene);
        void Render(const Core::Camera& camera, Core::Image* image);
    private:
        glm::vec3 TraceRay(const Ray& ray);
        glm::vec3 RayMiss();
    private:
        const Core::Scene& mScene;
    };
    
}

