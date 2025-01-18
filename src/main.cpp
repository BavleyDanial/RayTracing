#include <stdio.h>

#include <Image.h>
#include <ImageFile.h>

#include <glm/glm.hpp>

#define LOG(x, ...) printf(x, __VA_ARGS__) // Temporary, maybe I'll create a logging library for performance measurements

struct Ray {
    glm::vec3 org;
    glm::vec3 dir;
};

// TODO: when a ray misses, try using an environment map (cube map) instead of computing colors
glm::vec3 RayMiss(const Ray& ray) {
    glm::vec3 unitDir = glm::normalize(ray.dir);
    float a = 0.5f * (unitDir.y + 1.0f);
    return (1.0f - a) * glm::vec3(255.0f, 255.0f, 255.0f) + a * glm::vec3(238.0f, 175.0f, 97.0f);
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
        return RayMiss(ray);
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

int main() {
    RT::Image image(1920, 1080, 4);
    glm::vec2 viewport(2*image.aspectRatio, 2.0f);
    glm::vec3 cameraPos(0, 0, 1);

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            LOG("\rProgress: %i%%", (int)std::round(((float)y / image.height) * 100));

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

    RT::ImagePNG png(image);
    png.FlipVertical(true);
    png.Save("result.png");

    return 0;
}
