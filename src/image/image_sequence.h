#ifndef IMAGE_SEQUENCE_H
#define IMAGE_SEQUENCE_H

#include "image/image.h"
#include <string>
#include <utility>
#include <vector>

class ImageSequence {
public:
  ImageSequence() = default;

  ImageSequence(const ImageSequence &) = default;
  ImageSequence &operator=(const ImageSequence &) = default;
  ~ImageSequence() = default;

  void addImage(Image image, int delay) {
    mFrames.emplace_back(std::move(image), delay);
  }

  bool save(const std::string &output_path) const;

  int size() const noexcept { return mFrames.size(); }
  bool empty() const noexcept { return mFrames.empty(); }

private:
  std::vector<std::pair<Image, int>> mFrames;
};

#endif
