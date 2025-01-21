#include <Image.h>

namespace Core {

    void DrawPixel(Image* image, int pixelPos, const glm::vec4& color) {
        uint8_t r = static_cast<uint8_t>(255.0f * color.r);
        uint8_t g = static_cast<uint8_t>(255.0f * color.g);
        uint8_t b = static_cast<uint8_t>(255.0f * color.b);
        uint8_t a = static_cast<uint8_t>(255.0f * color.a);

        image->pixels[pixelPos] = r;
        image->pixels[pixelPos + 1] = g;
        image->pixels[pixelPos + 2] = b;
        image->pixels[pixelPos + 3] = a;
    }

    void DrawPixel(Image* image, int pixelPos, const glm::vec3& color) {
        uint8_t r = static_cast<uint8_t>(255.0f * color.r);
        uint8_t g = static_cast<uint8_t>(255.0f * color.g);
        uint8_t b = static_cast<uint8_t>(255.0f * color.b);
        
        image->pixels[pixelPos] = r;
        image->pixels[pixelPos + 1] = g;
        image->pixels[pixelPos + 2] = b;
    }

}
