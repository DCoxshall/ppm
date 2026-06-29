#pragma once

#include <vector>

#include "Pixel.h"

class ImageData {
   private:
    // Literal parts of the PPM specification.
    std::vector<Pixel> pixels;
    size_t width;
    size_t height;

   public:
    ImageData(size_t width, size_t height, std::vector<Pixel> blob_data) {
        this->width = width;
        this->height = height;
        this->pixels = blob_data;
    }

    size_t get_width() const {
        return width;
    }

    size_t get_height() const {
        return height;
    }

    std::vector<Pixel> get_pixels() const {
        return pixels;
    }
};
