#include <iostream>

#include <glm/glm.hpp>

#include <Image.h>
#include <ImageFile.h>

struct Ray {
    glm::vec3 org;
    glm::vec3 dir;
    float t;
};

glm::vec3 TraceRay(const Ray& ray) {
    glm::vec3 unitDir = glm::normalize(ray.dir);
    float a = 0.5f * (unitDir.y + 1.0f);
    return (1.0f - a) * glm::vec3(255.0f, 255.0f, 255.0f) + a * glm::vec3(238.0f, 175.0f, 97.0f);
}

int main() {
    std::ios_base::sync_with_stdio(false);

    RT::Image image(1920, 1080, 4);
    glm::vec2 viewport(2.0f * image.aspectRatio, 2.0f);
    glm::vec3 cameraPos(0);

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            std::clog << "\rProgress: " << std::round(((float)y / image.height) * 100) << '%';
            glm::vec2 fragPos = { (float)x / (image.width - 1), (float)y / (image.height - 1) };
            fragPos = fragPos * 2.0f - 1.0f;

            glm::vec3 rayDir = glm::vec3(fragPos.x, fragPos.y, -1.0f);
            Ray r(cameraPos, rayDir);

            glm::vec3 pixelColor = TraceRay(r);

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
