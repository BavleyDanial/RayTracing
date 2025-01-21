#pragma once

#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

namespace Core {

    struct Image {
        std::vector<uint8_t> pixels = {0};
        const uint32_t width = 0;
        const uint32_t height = 0;
        const float aspectRatio = 0;
        const uint8_t comps = 0;

        Image() = default;
        Image(uint32_t width, uint32_t height, uint8_t components = 3)
            :width(width), height(height), aspectRatio((float)width/height), comps(components) {
                pixels.resize(width * height * comps);
        }
    };

    void DrawPixel(Image* image, int pixelPos, const glm::vec4& color);
    void DrawPixel(Image* image, int pixelPos, const glm::vec3& color);

}

