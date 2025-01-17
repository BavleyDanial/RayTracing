#include <iostream>

#include <Image.h>
#include <ImageFile.h>

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 org;
    glm::vec3 dir;
    float t;
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
glm::vec3 TraceRay(const Ray& ray, const glm::vec3& camPos) {
    glm::vec3 circlePos(0.0f, 0.0f, 10.0f);
    float radius = 4.0f;

    glm::vec3 co = circlePos - camPos; // if the camera is moved somewhere offset the rendering as if the circle is in the origin
    float a = glm::dot(ray.dir, ray.dir);
    float b = 2.0f * glm::dot(ray.dir, co);
    float c = glm::dot(co, co) - radius*radius;
    float disc = b*b - 4*a*c;

    if (disc >= 0) {
        return glm::vec3(255, 0, 0);
    }

    return RayMiss(ray);
}

int main() {
    std::ios_base::sync_with_stdio(false);

    RT::Image image(1920, 1080, 4);
    glm::vec2 viewport(2*image.aspectRatio, 2.0f);
    glm::vec3 cameraPos(0);

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            std::clog << "\rProgress: " << std::round(((float)y / image.height) * 100) << '%';
            glm::vec2 fragPos = { (float)x / (image.width - 1), (float)y / (image.height - 1) };
            fragPos = fragPos * 2.0f - 1.0f;
            fragPos *= viewport;

            glm::vec3 rayDir = glm::vec3(fragPos.x, fragPos.y, 1.0f);
            Ray ray(cameraPos, rayDir);

            glm::vec3 pixelColor = TraceRay(ray, cameraPos);
            int pixelIdx = image.comps * (y * image.width + x);

            DrawPixel(image, pixelIdx, {pixelColor, 255});
            std::clog << std::flush;
        }
    }

    RT::ImagePNG png(image);
    png.FlipVertical(true);
    png.Save("result.png");

    return 0;
}
