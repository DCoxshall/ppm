#pragma once

#include <cstdint>

class Pixel {
   private:
    uint8_t r;
    uint8_t g;
    uint8_t b;

   public:
    Pixel(uint8_t r, uint8_t g, uint8_t b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};
