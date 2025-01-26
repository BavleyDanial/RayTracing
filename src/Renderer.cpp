#include "Scene.h"
#include "glm/geometric.hpp"
#include <Renderer.h>
#include <iostream>

namespace RT {

    Renderer::Renderer(const Core::Scene& scene)
        : mScene(scene) {}

    void Renderer::Render(const Core::Camera& camera, Core::Image* image) {
        for (uint32_t y = 0; y < image->height; y++) {
            for (uint32_t x = 0; x < image->width; x++) {
                const auto& rayDir = camera.GetRayDirections()[x + y * image->width];
                Ray ray(camera.GetPosition(), rayDir);

                glm::vec4 pixelColor{0};
                for (int i = 0; i < 2; i++) {
                    HitInfo hitInfo = TraceRay(ray);

                    if (hitInfo.hitDistance < 0) {
                        pixelColor += RayMiss() * (1 - (i * 0.25f));
                        break;
                    }
                    glm::vec4 color = FragShader(hitInfo) * (1 - (i * 0.25f));
                    pixelColor = glm::clamp(pixelColor + color, glm::vec4(0), glm::vec4(1.0f));
                    ray.org = hitInfo.worldPosition + 0.0001f * hitInfo.surfaceNormal;
                    ray.dir = glm::reflect(ray.dir, hitInfo.surfaceNormal);
                }

                int pixelIdx = image->comps * (y * image->width + x);
                DrawPixel(image, pixelIdx, pixelColor);
            }
        }
    }

    glm::vec4 Renderer::FragShader(const HitInfo& hitInfo) {
        const glm::vec3& hitNorm = hitInfo.surfaceNormal;
        const Core::Sphere& closestSphere = mScene.spheres[hitInfo.objIdx];

        // Diffuse Lighting
        // TODO: Make it support multiple lights and different colored lights
        float cosTerm = glm::max(glm::dot(hitNorm, -glm::normalize(mScene.directionalLights[0].direction)), 0.0f);
        glm::vec3 diff = mScene.directionalLights[0].intensity * cosTerm * mScene.materials[closestSphere.materialIndex].albedo;

        return { diff, 1.0f };
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
    Renderer::HitInfo Renderer::TraceRay(const Ray& ray) {
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

    glm::vec4 Renderer::RayMiss() {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

}
