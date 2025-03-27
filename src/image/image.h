#ifndef IMAGE_H
#define IMAGE_H

#include <array>
#include <string>

class Image {
public:
  Image(const std::string &imagePath);

  ~Image();

  bool load();

  int getWidth() const { return mImageWidth; }
  int getHeight() const { return mImageHeight; }
  int getChannels() const { return mChannels; }

  std::array<unsigned char, 3> getColorAt(int x, int y) const;

private:
  std::string mImagePath;
  int mImageWidth;
  int mImageHeight;
  int mChannels;
  unsigned char *mImageData;
};

#endif
