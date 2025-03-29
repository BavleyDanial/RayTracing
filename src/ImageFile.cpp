#include <ImageFile.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi/stb_image_write.h>

namespace Core {

    ImagePNG::ImagePNG(const Image* image) {
        mImage = std::make_unique<Image>(image->width, image->height, image->comps);
        mImage->pixels = image->pixels;
        for (int i = 0; i < mImage->width * mImage->height; i++) {
            mImage->pixels[i] = image->pixels[i];
        }
    }

    void ImagePNG::Load(const Image* image) {
        mImage.reset();
        mImage = std::make_unique<Image>(image->width, image->height, image->comps);
        mImage->pixels = image->pixels;
        for (int i = 0; i < mImage->width * mImage->height; i++) {
            mImage->pixels[i] = image->pixels[i];
        }
    }

    void ImagePNG::Save(const std::string& fileName) {
        stbi_flip_vertically_on_write(mFlipV);
        stbi_write_png((fileName + ".png").data(), mImage->width,
                        mImage->height, mImage->comps,
                        mImage->pixels.data(), mImage->width*mImage->comps);
    }

    void ImagePNG::FlipVertical(bool enabled) {
        mFlipV = enabled;
    }

}

