#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <vector>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Pixel;

typedef struct {
  std::vector<Pixel> pixels;
  int width;
  int height;
} Image;

static Image load_ppm(const std::string& path) {
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
  // P6).

  std::string magic;
  uint32_t width;
  uint32_t height;
  uint16_t maxval;

  std::ifstream file(path);

  if (!file) throw std::runtime_error("Failed to open file: " + path + ".");

  file >> magic;
  if (magic != "P3")
    throw std::runtime_error(
        "File's magic number didn't exist or wasn't 'P3'.");

  file >> width;
  if (!file) throw std::runtime_error("File's value for `width` was invalid.");

  file >> height;
  if (!file) throw std::runtime_error("File's value for `height` was invalid.");

  file >> maxval;
  if (!file || maxval == 0)
    throw std::runtime_error("File's value for `maxval` was invalid.");

  if (width > 0 && height > UINT32_MAX / width) {
    throw std::runtime_error("PPM: image size overflow");
  }
  uint32_t size = width * height;

  Image img;
  img.width = width;
  img.height = height;
  img.pixels.reserve(size);

  for (uint32_t i = 0; i < size; i++) {
    uint16_t r, g, b;
    if (!(file >> r >> g >> b))
      throw std::runtime_error("Incorrect or malformed pixel data.");

    // We need to map [0, 65535] to [0, 255].
    uint8_t R, G, B;
    R = static_cast<uint8_t>(r * 255.0 / maxval);
    G = static_cast<uint8_t>(g * 255.0 / maxval);
    B = static_cast<uint8_t>(b * 255.0 / maxval);

    img.pixels.push_back(Pixel{R, G, B});
  }

  file >> std::ws;
  if (file.peek() != std::char_traits<char>::eof())
    throw std::runtime_error("PPM: trailing data");

  // Ensure that there is nothing left in the file at all.
  if (file.peek() != std::char_traits<char>::eof())
    throw std::runtime_error("Unexpected trailing data.");

  return img;
}

static int show_image(const Image& img) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("PPM Viewer", img.width, img.height, 0);

  if (!window) {
    std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

  if (!renderer) {
    std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Texture* texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STATIC, img.width, img.height);

  if (!texture) {
    std::cerr << "Texture creation failed: " << SDL_GetError() << "\n";
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Upload pixel data
  SDL_UpdateTexture(texture, nullptr, img.pixels.data(), img.width * 3);

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

  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ppm [filename]" << std::endl;
    return 1;
  }

  std::string filename = argv[1];

  Image img;
  try {
    img = load_ppm(filename);
  } catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return 1;
  }

  show_image(img);
  return 0;
}