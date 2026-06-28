#pragma once
#include <vector>

#include "ImageData.h"
#include "Token.h"

enum MagicNumberVariant { P3, P6 };

// The job of the parser is to take a vector of tokens and create image data
// from those tokens, or throw an error if the tokens received do not represent
// an image represented in valid PPM format.
class Parser {
   private:
    std::vector<Token> tokens;
    size_t at;

    enum MagicNumberVariant parse_magic_number();
    size_t parse_dimension();
    uint16_t parse_maxval();
    std::vector<Pixel> parse_blob(enum MagicNumberVariant magic, size_t width,
                                  size_t height, uint16_t maxval);
    std::vector<Pixel> parse_binary_blob(size_t width, size_t height,
                                         uint16_t maxval);
    std::vector<Pixel> parse_ascii_blob(size_t width, size_t height,
                                        uint16_t maxval);

    bool at_end();

   public:
    Parser(std::vector<Token> tokens) {
        this->tokens = tokens;
        this->at = 0;
    }

    ImageData parse();
};