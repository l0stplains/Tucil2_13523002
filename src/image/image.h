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
  std::string getFileExt() const { return mFileExt; }

  bool load();
  bool save(const std::string &outputPath);
  long long estimateFileSize() const;

  int getWidth() const { return mImageWidth; }
  int getHeight() const { return mImageHeight; }
  int getChannels() const { return mChannels; }
  long long getFileSize() const { return mFileSize; }
  unsigned char *getImageData() const { return mImageData; }

  std::array<unsigned char, 3> getColorAt(int x, int y) const;
  unsigned char getAlphaAt(int x, int y) const;

  int getIdxAt(int x, int y, int channel) const;

  long long getChannelBlockSum(int x, int y, int width, int height,
                               int channel) const;
  long long getChannelSquareBlockSum(int x, int y, int width, int height,
                                     int channel) const;

  void setColorAt(int x, int y, unsigned char r, unsigned char g,
                  unsigned char b);
  void setAlphaAt(int x, int y, unsigned char a);

  void setBlockColorAt(int x, int y, int width, int height, unsigned char r,
                       unsigned char g, unsigned char b);
  void computeSummedSquareTable();
  void computeSummedAreaTable();

private:
  std::string mImagePath;
  std::string mFileExt;
  int mImageWidth;
  int mImageHeight;
  int mChannels;
  unsigned char *mImageData;

  long long mFileSize;

  long long *mSummedAreaTable;
  long long *mSummedSquareTable;
};

#endif
