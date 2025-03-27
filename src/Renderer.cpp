#include "Scene.h"
#include "glm/ext/scalar_constants.hpp"
#include <Renderer.h>

namespace RT {

    Renderer::Renderer(const Core::Scene& scene)
        : mScene(scene), mRNG(1) {}

    void Renderer::Render(const Core::Camera& camera, Core::Image* image, uint32_t frame) {
        for (uint32_t y = 0; y < image->height; y++) {
            for (uint32_t x = 0; x < image->width; x++) {
                const auto& rayDir = camera.GetRayDirections()[x + y * image->width];
                Ray ray(camera.GetPosition(), rayDir);

                mRNG = x + y * image->width;
                glm::vec3 totalColor{0};
                for (int i = 0; i < raysPerPixel; i++) {
                    totalColor += TraceRay(ray);
                }
                totalColor /= (float)raysPerPixel;
                totalColor = glm::clamp(totalColor, glm::vec3(0.0f), glm::vec3(1.0f));

                int pixelIdx = image->comps * (y * image->width + x);
                DrawPixel(image, pixelIdx, {totalColor, 1});
            }
        }
    }

    glm::vec3 Renderer::TraceRay(Ray& ray) {
        glm::vec3 rayColor{1};
        glm::vec3 incomingLight{0};

        for (int i = 0; i < bounceLimit; i++) {
            HitInfo hitInfo = RayIntersectionTest(ray);
            if (hitInfo.objIdx < 0) {
                break;
            }

            const glm::vec3& hitNorm = hitInfo.surfaceNormal;
            const Core::Sphere& closestSphere = mScene.spheres[hitInfo.objIdx];

            ray.org = hitInfo.worldPosition + hitNorm * 0.001f;
            ray.dir = glm::normalize(hitNorm + RandomDirection(mRNG));

            Core::Material mat = mScene.materials[closestSphere.materialIndex];
            glm::vec3 emittedLight = mat.emissionColor * mat.emissionStrength;
            incomingLight += emittedLight * rayColor;
            rayColor *= mat.albedo;
        }

        return incomingLight;
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
    Renderer::HitInfo Renderer::RayIntersectionTest(const Ray& ray) {
        // TODO: Make it support multiple kinds of objects other than spheres
        const Core::Sphere* closestSphere = nullptr;
        float tmin = FLT_MAX;
        int objIdx = -1;
        for (int i = 0; i < mScene.spheres.size(); i++) {
            const auto& sphere = mScene.spheres[i];
            glm::vec3 origin = ray.org - sphere.position; // if the camera is moved somewhere offset the rendering as if the circle is at the origin of the camera
            float a = glm::dot(ray.dir, ray.dir);
            float b = 2.0f * glm::dot(origin, ray.dir);
            float c = glm::dot(origin, origin) - sphere.radius*sphere.radius;
            float discriminant = b*b - 4*a*c;

            // (-b +- sqrt(b^2 - 4ac))/2a
            if (discriminant < 0) {
                continue;
            }

            float t0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
            //float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
            if (t0 < tmin && t0 >= 0) {
                closestSphere = &sphere;
                tmin = t0;
                objIdx = i;
            }
        }

        HitInfo hitInfo{};
        if (!closestSphere) {
            return hitInfo;
        }

        hitInfo.worldPosition = ray.org + tmin * ray.dir;
        hitInfo.surfaceNormal = glm::normalize(hitInfo.worldPosition - closestSphere->position);
        hitInfo.hitDistance = tmin;
        hitInfo.objIdx = objIdx;
        return hitInfo;
    }

    glm::vec3 Renderer::RayMiss() {
        return { 0.0f, 0.0f, 0.0f };
    }

    float Renderer::RandomValue(uint32_t& state) {
        state = NextRandom(state);
        return (float)state / (float)4294967295.0; // 2^32 - 1

    }

    uint32_t Renderer::NextRandom(uint32_t& state) {
        state = state * 747796405 + 2891336453;
        uint32_t result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
        result = (result >> 22) ^ result;
        return result;
    }

    float Renderer::RandomValueNormalDistribution(uint32_t& state) {
        float theta = 2 * glm::pi<float>() * RandomValue(state);
        float rho = glm::sqrt(-2 * glm::log(RandomValue(state)));
        return rho * glm::cos(theta);
    }

    glm::vec3 Renderer::RandomDirection(uint32_t& state) {
        float x = RandomValueNormalDistribution(state);
        float y = RandomValueNormalDistribution(state);
        float z = RandomValueNormalDistribution(state);
        return glm::normalize(glm::vec3(x, y, z));
    }

}
