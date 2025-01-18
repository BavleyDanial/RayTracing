#include <Image.h>

namespace RT {

    void DrawPixel(Image& image, int pixelPos, const glm::vec4& color) {
        uint8_t r = static_cast<uint8_t>(color.r);
        uint8_t g = static_cast<uint8_t>(color.g);
        uint8_t b = static_cast<uint8_t>(color.b);
        uint8_t a = static_cast<uint8_t>(color.a);

        image.pixels[pixelPos] = r;
        image.pixels[pixelPos + 1] = g;
        image.pixels[pixelPos + 2] = b;
        image.pixels[pixelPos + 3] = a;
    }

    void DrawPixel(Image& image, int pixelPos, const glm::vec3& color) {
        image.pixels[pixelPos] = color.r;
        image.pixels[pixelPos + 1] = color.g;
        image.pixels[pixelPos + 2] = color.b;
    }

}
