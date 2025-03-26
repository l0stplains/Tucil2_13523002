#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fs = std::filesystem;

int main() {
  std::string filename;
  std::cout << "Enter image file path: ";
  std::getline(std::cin, filename);

  // convert to absolute
  fs::path imagePath = fs::absolute(filename);

  // check
  if (!fs::exists(imagePath)) {
    std::cerr << "Error: File does not exist.\n";
    return 1;
  }

  int width, height, channels;

  // load
  std::unique_ptr<unsigned char, decltype(&stbi_image_free)> img(
      stbi_load(imagePath.string().c_str(), &width, &height, &channels, 0),
      stbi_image_free);

  if (!img) {
    std::cerr << "Failed to load image: " << stbi_failure_reason() << "\n";
    return 1;
  }

  std::cout << "Image loaded: " << width << "x" << height << " with "
            << channels << " channels\n";

  // TODO: ganbatte

  return 0;
}
