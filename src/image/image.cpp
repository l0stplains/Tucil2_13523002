#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Image::Image(const std::string &imagePath)
    : mImagePath(imagePath), mImageWidth(0), mImageHeight(0), mChannels(0),
      mImageData(nullptr) {}

Image::~Image() {
  if (mImageData) {
    stbi_image_free(mImageData);
  }
}

bool Image::load() {
  mImageData =
      stbi_load(mImagePath.c_str(), &mImageWidth, &mImageHeight, &mChannels, 0);
  if (!mImageData) {
    std::cerr << "Error: " << stbi_failure_reason() << std::endl;
    return 1;
  }
  if (mChannels < 3) {
    std::cerr << "Non-RGB image not supported" << std::endl;
    return 1;
  }
  std::cout << "Loaded image (" << mImageWidth << "x" << mImageHeight
            << ") with " << mChannels << " channels." << std::endl;
  return 0;
}

std::array<unsigned char, 3> Image::getColorAt(int x, int y) const {
  std::array<unsigned char, 3> color = {0, 0, 0}; // RGB
  if (x < 0 || x >= mImageWidth || y < 0 || y >= mImageHeight)
    return color;

  int index = y * (mImageWidth * mChannels) + x * mChannels;
  color[0] = mImageData[index];
  color[1] = mImageData[index + 1];
  color[2] = mImageData[index + 2];
  return color;
}
