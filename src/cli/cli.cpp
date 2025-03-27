#include "cli.h"
#include "image/image.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

bool CLI::start() {
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

  Image userImage(imagePath);

  if (userImage.load()) {
    std::cerr << "Failed to load image: " << imagePath << "\n";
    return 1;
  }

  // TODO: ganbatte

  return 0;
}
