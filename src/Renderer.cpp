#include "Renderer.h"

#include <iostream>
#include <ostream>

namespace RT {
    
    Renderer::Renderer(const Core::Scene& scene)
        : mScene(scene) {}

    void Renderer::Render(const Core::Camera& camera, Core::Image& image) {
        for (uint32_t y = 0; y < image.height; y++) {
            for (uint32_t x = 0; x < image.width; x++) {
                glm::vec2 fragPos = { (float)x / (image.width - 1), (float)y / (image.height - 1) };
                fragPos = fragPos * 2.0f - 1.0f;
                fragPos *= camera.GetViewport();

                glm::vec3 rayDir = glm::vec3(fragPos.x, fragPos.y, -1.0f);
                Ray ray(camera.GetPosition(), rayDir);

                glm::vec3 pixelColor = TraceRay(ray);
                pixelColor = glm::clamp(pixelColor, glm::vec3(0), glm::vec3(255));

                int pixelIdx = image.comps * (y * image.width + x);
                DrawPixel(image, pixelIdx, {pixelColor, 255});
            }
        }
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
    glm::vec3 Renderer::TraceRay(const Ray& ray) {
        // TODO: Make it support multiple kinds of objects other than spheres
        Core::Sphere* closestSphere = nullptr;
        float tmin = FLT_MAX;
        for (auto& sphere : mScene.spheres) {
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
            if (t0 < tmin) {
                closestSphere = &sphere;
                tmin = t0;
            }
        }

        if (!closestSphere) {
            return RayMiss();
        }

        glm::vec3 hit0 = (ray.org - closestSphere->position) + tmin * ray.dir;
        //glm::vec3 hit1 = ray.org + t1 * ray.dir;
        glm::vec3 hitNorm = glm::normalize(hit0);
        
        // Diffuse Lighting
        // TODO: Make it support multiple lights and different colored lights
        float cosTerm = glm::max(glm::dot(hitNorm, -glm::normalize(mScene.directionalLights[0].direction)), 0.0f);
        return mScene.directionalLights[0].intensity * cosTerm * mScene.materials[closestSphere->materialIndex].albedo;
    }

    glm::vec3 Renderer::RayMiss() {
        return glm::vec3(0);
    }
    
}
