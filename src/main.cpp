// The file format for a PPM file contains the following, in ASCII  decimal
// text:
// 1. The magic number "P3".
// 2. Whitespace (blanks, tabs, CRs and LFs).
// 3. The image's width.
// 4. Whitespace.
// 5. The image's height.
// 6. Whitespace.
// 7. The maximum color value ("maxval"), where maxval > 0 and maxval <
// 65536. It is pretty common for maxval to be 255.
// 8. A single whitespace character, usually a newline.
// 9. A raster of height rows, in order from top to bottom. Each row
// consists of `width` pixels, in order from left to right. Each pixel is a
// triplet of red, green and blue samples, in decimal (since this is P3, not
// P6
// Up until step 8, we also allow lines to be metadata lines. If a line
// starts with #, we simply skip this line.

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "ImageData.h"
#include "Scanner.h"
#include "Token.h"

std::vector<uint8_t> read_data_from_file(std::ifstream& file) {
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>());
}

void check_cmd_line_args(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::runtime_error(
            "Incorrect number of command line arguments. Usage: ppm "
            "[filename]");
    }
}

std::ifstream open_file(std::string file_path_str) {
    std::filesystem::path file_path = file_path_str;
    if (!std::filesystem::exists(file_path))
        throw std::runtime_error("File does not exist.");

    std::ifstream file(file_path, std::ios::binary);
    if (!file) throw std::runtime_error("File could not be opened.");

    return file;
}

int main(int argc, char* argv[]) {
    try {
        check_cmd_line_args(argc, argv);
        std::ifstream file = open_file(argv[1]);
        Scanner scanner = Scanner(read_data_from_file(file));
        std::vector<Token> tokens = scanner.generate_tokens();
        for (Token t : tokens) {
            token_show(t);
        }
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}