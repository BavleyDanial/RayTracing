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
        void Render(const Core::Camera& camera, Core::Image* image, uint32_t frame);
    public:
        int bounceLimit = 1;
        int raysPerPixel = 1;

    private:
        struct HitInfo {
            glm::vec3 worldPosition;
            glm::vec3 surfaceNormal;
            float hitDistance = -1.0f;
            int objIdx = -1;
        };

    private:
        glm::vec3 TraceRay(Ray& ray);
        HitInfo RayIntersectionTest(const Ray& ray);
        HitInfo FindClosestObj(const Ray& ray);
        glm::vec3 RayMiss();

        uint32_t NextRandom(uint32_t& state);
        float RandomValue(uint32_t& state);
        glm::vec3 RandomDirection(uint32_t& state);
        float RandomValueNormalDistribution(uint32_t& state);

    private:
        const Core::Scene& mScene;
        uint32_t mRNG;
    };

}

