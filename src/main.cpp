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
#include "Parser.h"
#include "Scanner.h"
#include "Token.h"

std::vector<uint8_t> read_data_from_file(std::ifstream& file) {
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>());
}

static void check_cmd_line_args(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::runtime_error(
            "Incorrect number of command line arguments. Usage: ppm "
            "[filename]");
    }
}

static std::ifstream open_file(std::string file_path_str) {
    std::filesystem::path file_path = file_path_str;
    if (!std::filesystem::exists(file_path))
        throw std::runtime_error("File does not exist.");

    std::ifstream file(file_path, std::ios::binary);
    if (!file) throw std::runtime_error("File could not be opened.");

    return file;
}

static void show_image(const ImageData& img, std::string file_name) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("SDL init failed: " +
                                 std::string(SDL_GetError()));
    }

    SDL_Window* window = SDL_CreateWindow(file_name.c_str(), img.get_height(),
                                          img.get_height(), 0);

    if (!window) {
        SDL_Quit();
        throw std::runtime_error("Window creation failed: " +
                                 std::string(SDL_GetError()));
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Renderer creation failed: " +
                                 std::string(SDL_GetError()));
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STATIC,
                                             img.get_width(), img.get_height());

    if (!texture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Texture creation failed: " +
                                 std::string(SDL_GetError()));
    }

    // Upload pixel data
    SDL_UpdateTexture(texture, nullptr, img.get_pixels().data(),
                      img.get_width() * 3);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    try {
        check_cmd_line_args(argc, argv);
        std::ifstream file = open_file(argv[1]);

        Scanner scanner = Scanner(read_data_from_file(file));
        std::vector<Token> tokens = scanner.generate_tokens();

        Parser parser = Parser(tokens);
        ImageData img_data = parser.parse();

        show_image(img_data, "PPM Viewer");

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}