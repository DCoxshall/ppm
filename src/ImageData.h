#pragma once

#include <vector>

#include "Pixel.h"

class ImageData {
   private:
    // Literal parts of the PPM specification.
    std::vector<Pixel> pixels;
    size_t width;
    size_t height;

    // Used for scaling down colours where the scale goes above 255.
    uint16_t maxval;

   public:
    ImageData(size_t width, size_t height, uint16_t maxval) {
        this->width = width;
        this->height = height;
        this->pixels = std::vector<Pixel>();
        this->maxval = maxval;
    }
};
