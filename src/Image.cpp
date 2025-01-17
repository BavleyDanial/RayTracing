#include <Image.h>

namespace RT {

    void DrawPixel(Image& image, int pixelPos, const glm::vec4& color) {
        image.pixels[pixelPos] = color.r;
        image.pixels[pixelPos + 1] = color.g;
        image.pixels[pixelPos + 2] = color.b;
        image.pixels[pixelPos + 3] = color.a;
    }

    void DrawPixel(Image& image, int pixelPos, const glm::vec3& color) {
        image.pixels[pixelPos] = color.r;
        image.pixels[pixelPos + 1] = color.g;
        image.pixels[pixelPos + 2] = color.b;
    }

}
