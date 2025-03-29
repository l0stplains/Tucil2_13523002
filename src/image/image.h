#ifndef IMAGE_H
#define IMAGE_H

#include <array>
#include <cstring>
#include <string>

class Image {
public:
  Image(const std::string &imagePath);
  Image(const Image &other);

  Image &operator=(const Image &other);

  ~Image();

  std::string getImagePath() const { return mImagePath; }
  std::string getFileType() const { return mFileType; }

  bool load();
  bool save(const std::string &outputPath) const;

  int getWidth() const { return mImageWidth; }
  int getHeight() const { return mImageHeight; }
  int getChannels() const { return mChannels; }
  unsigned char *getImageData() const { return mImageData; }

  std::array<unsigned char, 3> getColorAt(int x, int y) const;

  int getIdxAt(int x, int y, int channel) const;

  long long getChannelBlockSum(int x, int y, int width, int height,
                               int channel) const;
  long long getChannelSquareBlockSum(int x, int y, int width, int height,
                                     int channel) const;

  void setColorAt(int x, int y, unsigned char r, unsigned char g,
                  unsigned char b);

private:
  std::string mImagePath;
  std::string mFileType;
  int mImageWidth;
  int mImageHeight;
  int mChannels;
  unsigned char *mImageData;
  long long *mSummedAreaTable;
  long long *mSummedSquareTable;

  void computeSummedSquareTable();
  void computeSummedAreaTable();
};

#endif
