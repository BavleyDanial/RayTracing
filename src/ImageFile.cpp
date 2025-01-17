#include <ImageFile.h>
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi/stb_image_write.h>

namespace RT {

    ImagePNG::ImagePNG(const Image& image) {
        mImage = std::make_unique<Image>(image.width, image.height, image.comps);
        mImage->pixels = image.pixels;
    }

    void ImagePNG::Load(const Image& image) {
        mImage.reset();
        mImage = std::make_unique<Image>(image.width, image.height, image.comps);
        mImage->pixels = image.pixels;
    }

    void ImagePNG::Save(const std::string& fileName) {
        std::ofstream out_file(fileName);
        out_file.close();

        stbi_flip_vertically_on_write(mFlipV);
        stbi_write_png(fileName.data(), mImage->width,
                        mImage->height, mImage->comps,
                        mImage->pixels.data(), mImage->width*mImage->comps);
    }

    void ImagePNG::FlipVertical(bool enabled) {
        mFlipV = enabled;
    }

}

