#include <fstream>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi/stb_image_write.h>
#include <glm/glm.hpp>

struct Image {
    std::vector<uint8_t> pixels;
    const uint32_t width;
    const uint32_t height;
    const uint8_t comps;

    Image(uint32_t width, uint32_t height, uint8_t components = 3)
        :width(width), height(height), comps(components) {
            pixels.resize(width * height * comps);
        }
};

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

class ImagePNG {
public:
    ImagePNG(const Image& image)
        :image(image) {}

    void SaveImage(std::string_view file) {
        std::string fileName(file);
        std::ofstream out_file(fileName);
        out_file.close();
        stbi_write_png(file.data(), image.width, image.height, image.comps, image.pixels.data(), image.width*image.comps);
    }

private:
    Image image;
};

int main() {
    Image image(1920, 1080, 4);

    std::ios_base::sync_with_stdio(false);
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            std::clog << "\rProgress: " << std::ceil(((float)y / image.height) * 100) << '%';
            float r = 255.999f * (float(x) / (image.width-1));
            float g = 255.999f * (float(y) / (image.height-1));
            auto b = 0.0;
            int pixelPos = image.comps * (y * image.width + x);
            DrawPixel(image, pixelPos, {r, g, b, 255});
            std::clog << std::flush;
        }
    }

    ImagePNG png(image);
    png.SaveImage("result.png");

    return 0;
}
