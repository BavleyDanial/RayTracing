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
        struct HitInfo {
            glm::vec3 worldPosition;
            glm::vec3 surfaceNormal;
            float hitDistance = -1.0f;
            int objIdx = -1;
        };
    private:
        glm::vec4 TraceRay(Ray& ray);
        HitInfo RayIntersectionTest(const Ray& ray);
        HitInfo FindClosestObj(const Ray& ray);
        glm::vec4 RayMiss();
    private:
        const Core::Scene& mScene;
    };

}

