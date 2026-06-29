#include "Parser.h"

#include <sstream>
#include <stdexcept>

#include "Pixel.h"

bool Parser::at_end() {
    return at == tokens.size();
}

// Verifies that we're currently pointing at a token of the expected type.
static void prelim_check(std::vector<Token> tokens, size_t at,
                         TokenType expected_type,
                         std::string expected_type_str) {
    // Verify that we're not already at the end of the token vector.
    if (at >= tokens.size())
        throw std::runtime_error(expected_type + " not found.");

    // Verify that the first token is indeed a magic number.
    if (tokens[at].type != expected_type)
        throw std::runtime_error(expected_type + " not found.");
}

enum MagicNumberVariant Parser::parse_magic_number() {
    prelim_check(tokens, at, MagicNumber, "Magic number");

    enum MagicNumberVariant magic;

    // Determine whether the given lexeme is "P3" or "P6".
    if (tokens[at].lexeme == "P3") {
        magic = P3;
    } else if (tokens[at].lexeme == "P6") {
        magic = P6;
    } else {
        throw std::runtime_error("Magic number must be either 'P3' or 'P6'");
    }

    // Move to the next token.
    at++;

    return magic;
}

size_t Parser::parse_dimension() {
    prelim_check(tokens, at, Number, "Image dimension ");

    // Determine that we have a correct and valid size_t. This code accounts for
    // negatives but does not need to, since the scanning phase ensures that
    // negative numbers won't get through.
    std::stringstream ss(tokens[at].lexeme);
    size_t value;

    if (!(ss >> value) || !ss.eof())
        throw std::runtime_error("Invalid value for image dimensions.");

    // Move to next token.
    at++;

    return value;
}

uint16_t Parser::parse_maxval() {
    prelim_check(tokens, at, Number, "Maxval ");

    // Same code as for parsing image dimensions, but with a uint16_t rather
    // than a size_t.
    std::stringstream ss(tokens[at].lexeme);
    uint16_t value;

    if (!(ss >> value) || !ss.eof() || value == 0)
        throw std::runtime_error("Invalid value for maxval.");

    // Move to next token.
    at++;

    return value;
}

static std::vector<Pixel> parse_raw_blob(std::vector<uint16_t> raw_data,
                                         size_t width, size_t height,
                                         uint16_t maxval) {
    std::vector<Pixel> pixels;

    size_t pixel_count = width * height;
    size_t expected_data_size = pixel_count * 3;  // 3 values for each pixel

    if (raw_data.size() != pixel_count * 3)
        throw std::runtime_error(
            "Invalid blob size for given image dimensions.");

    for (size_t i = 0; i < expected_data_size; i += 3) {
        uint16_t r, g, b;
        r = raw_data[i];
        g = raw_data[i + 1];
        b = raw_data[i + 2];

        // We need to map [0, 65535] to [0, 255]. Parser ensures that maxval !=
        // 0.
        uint8_t R, G, B;
        R = static_cast<uint8_t>(255 * static_cast<uint32_t>(r) / maxval);
        G = static_cast<uint8_t>(255 * static_cast<uint32_t>(g) / maxval);
        B = static_cast<uint8_t>(255 * static_cast<uint32_t>(b) / maxval);

        pixels.push_back(Pixel{R, G, B});
    }

    return pixels;
}

std::vector<Pixel> Parser::parse_blob(enum MagicNumberVariant magic,
                                      size_t width, size_t height,
                                      uint16_t maxval) {
    prelim_check(tokens, at, Blob, "Blob ");

    if (magic == P3) {
        return Parser::parse_ascii_blob(width, height, maxval);
    } else {
        return Parser::parse_binary_blob(width, height, maxval);
    }
}

std::vector<Pixel> Parser::parse_binary_blob(size_t width, size_t height,
                                             uint16_t maxval) {
    std::vector<uint16_t> blob;
    std::string raw = tokens[at].lexeme;

    // Represents whether this binary blob is using 16 bits for each RGB
    // component.
    bool double_width_pixels = maxval > 255;

    // Sanity check the size of the blob.
    if ((double_width_pixels && (raw.size() != width * height * 3)) ||
        (double_width_pixels && (raw.size() != width * height * 6)))
        throw std::runtime_error("Invalid binary blob size.");

    if (double_width_pixels) {
        unsigned char first_half;
        bool on_first_half = true;

        for (unsigned char byte : raw) {
            if (on_first_half) {
                first_half = byte;
            } else {
                blob.push_back(static_cast<uint16_t>(first_half) << 8 + byte);
            }
            on_first_half = on_first_half ? false : true;
        }
    } else {
        for (unsigned char byte : raw) {
            blob.push_back(static_cast<uint16_t>(byte));
        }
    }

    return parse_raw_blob(blob, width, height, maxval);
}

std::vector<Pixel> Parser::parse_ascii_blob(size_t width, size_t height,
                                            uint16_t maxval) {
    std::istringstream stream(tokens[at].lexeme);
    std::vector<uint16_t> raw;
    uint16_t value;

    while (stream >> value) {
        raw.push_back(value);
    }

    if (!stream.eof())
        throw std::runtime_error("Invalid number in ASCII blob.");

    return parse_raw_blob(raw, width, height, maxval);
}

ImageData Parser::parse() {
    enum MagicNumberVariant magic_type = parse_magic_number();
    size_t width = parse_dimension();
    size_t height = parse_dimension();
    uint16_t maxval = parse_maxval();
    std::vector<Pixel> pixels = parse_blob(magic_type, width, height, maxval);

    return ImageData(width, height, pixels);
}
