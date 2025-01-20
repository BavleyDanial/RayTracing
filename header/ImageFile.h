#pragma once

#include <Image.h>

#include <string>
#include <memory>

namespace Core {

    class ImagePNG {
    public:
        ImagePNG() = default;
        ImagePNG(const Image& image);

        void Load(const Image& image);
        void Save(const std::string& fileName);
        void FlipVertical(bool enabled);
    private:
        std::unique_ptr<Image> mImage;
        bool mFlipV = false;
    };

}

