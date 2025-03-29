#include "image_sequence.h"
#include "gif.h"
#include "utils/debug.h"
#include <stdexcept>
#include <vector>

namespace {
std::vector<uint8_t> convert_to_rgba(const Image &img) {
  const int width = img.getWidth();
  const int height = img.getHeight();
  const int src_channels = img.getChannels();
  const uint8_t *src = img.getImageData();

  std::vector<uint8_t> dst(width * height * 4);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const size_t src_idx = (y * width + x) * src_channels;
      const size_t dst_idx = (y * width + x) * 4;

      for (int c = 0; c < 3; ++c) {
        dst[dst_idx + c] = src[src_idx + c];
      }

      dst[dst_idx + 3] = (src_channels == 4) ? src[src_idx + 3] : 0xFF;
    }
  }
  return dst;
}
} // namespace

bool ImageSequence::save(const std::string &output_path) const {
  DEBUG_TIMER("Saving Gif");
  if (mFrames.empty())
    return false;

  const auto &[first_image, first_delay] = mFrames.front();
  const int width = first_image.getWidth();
  const int height = first_image.getHeight();

  GifWriter writer;
  if (!GifBegin(&writer, output_path.c_str(), width, height, first_delay)) {
    return false;
  }

  try {
    for (const auto &[img, delay] : mFrames) {
      if (img.getWidth() != width || img.getHeight() != height) {
        throw std::runtime_error("All images must have identical dimensions");
      }

      // Convert to 4-channel RGBA format
      const auto pixel_data = convert_to_rgba(img);

      if (!GifWriteFrame(&writer, pixel_data.data(), width, height, delay)) {
        throw std::runtime_error("GIF frame write failed");
      }
    }
  } catch (...) {
    GifEnd(&writer);
    return false;
  }

  return GifEnd(&writer);
}
