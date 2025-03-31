#pragma once

#include <Scene.h>

#include <Image.h>
#include <Camera.h>
#include <memory>

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
        void OnResize(uint32_t width, uint32_t height);
    public:
        int bounceLimit = 8;
        float gamma = 2.2f;
        float exposure = 1.0f;
        bool doGammaCorrection = true;
        bool doToneMapping = true;
        
    private:
        struct HitInfo {
            glm::vec3 worldPosition;
            glm::vec3 surfaceNormal;
            float hitDistance = -1.0f;
            int objIdx = -1;
        };

    private:
        glm::vec3 TraceRay(const Ray& ray);
        HitInfo RayIntersectionTest(const Ray& ray);
        glm::vec3 RayMiss();

        glm::vec3 ApplyGammaCorrection(const glm::vec3& color);
        glm::vec3 ApplyToneMapping(const glm::vec3& x);

        uint32_t NextRandom(uint32_t& state);
        float RandomValue(uint32_t& state);
        glm::vec3 RandomDirection(uint32_t& state);
        float RandomValueNormalDistribution(uint32_t& state);

    private:
        const Core::Scene& mScene;
        glm::vec3* mAccumulatedData = nullptr;
        std::vector<uint32_t> mVerticalIter;
        std::vector<uint32_t> mHorizontalIter;
        inline static thread_local uint32_t mRNG = 1;
    };

}

