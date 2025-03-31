#include "image.h"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_writer.h"
// #include "utils/debug.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Image::Image(const std::string &imagePath)
    : mImagePath(fs::absolute(imagePath).string()), mFileExt(""),
      mImageWidth(0), mImageHeight(0), mChannels(0), mImageData(nullptr),
      mFileSize(0), mSummedAreaTable(nullptr), mSummedSquareTable(nullptr) {}

Image::Image(const Image &other)
    : mImagePath(other.mImagePath), mFileExt(other.mFileExt),
      mImageWidth(other.mImageWidth), mImageHeight(other.mImageHeight),
      mChannels(other.mChannels), mImageData(nullptr),
      mFileSize(other.mFileSize), mSummedAreaTable(nullptr),
      mSummedSquareTable(nullptr) {

  if (other.mImageData) {
    int dataSize = mImageWidth * mImageHeight * mChannels;
    mImageData = new unsigned char[dataSize];
    std::memcpy(mImageData, other.mImageData, dataSize);
  }

  if (other.mSummedAreaTable) {
    int tableSize = mImageWidth * mImageHeight * mChannels;
    mSummedAreaTable = new long long[tableSize];
    std::memcpy(mSummedAreaTable, other.mSummedAreaTable,
                tableSize * sizeof(long long));
  }
  if (other.mSummedSquareTable) {
    int tableSize = mImageWidth * mImageHeight * mChannels;
    mSummedSquareTable = new long long[tableSize];
    std::memcpy(mSummedSquareTable, other.mSummedSquareTable,
                tableSize * sizeof(long long));
  }
}

Image &Image::operator=(const Image &other) {
  if (this != &other) {
    delete[] mImageData;
    delete[] mSummedAreaTable;
    delete[] mSummedSquareTable;

    mImagePath = other.mImagePath;
    mImageWidth = other.mImageWidth;
    mImageHeight = other.mImageHeight;
    mFileExt = other.mFileExt;
    mChannels = other.mChannels;

    if (other.mImageData) {
      int dataSize = mImageWidth * mImageHeight * mChannels;
      mImageData = new unsigned char[dataSize];
      std::memcpy(mImageData, other.mImageData, dataSize);
    } else {
      mImageData = nullptr;
    }

    if (other.mSummedAreaTable) {
      int tableSize = mImageWidth * mImageHeight * mChannels;
      mSummedAreaTable = new long long[tableSize];
      std::memcpy(mSummedAreaTable, other.mSummedAreaTable,
                  tableSize * sizeof(long long));
    } else {
      mSummedAreaTable = nullptr;
    }
    if (other.mSummedSquareTable) {
      int tableSize = mImageWidth * mImageHeight * mChannels;
      mSummedSquareTable = new long long[tableSize];
      std::memcpy(mSummedSquareTable, other.mSummedSquareTable,
                  tableSize * sizeof(long long));
    } else {
      mSummedSquareTable = nullptr;
    }
  }
  return *this;
}

Image::~Image() {
  if (mImageData) {
    stbi_image_free(mImageData);
  }
  delete[] mSummedAreaTable;
  delete[] mSummedSquareTable;
}

bool Image::load() {

  fs::path filepath(mImagePath);

  mFileExt = filepath.extension().string();
  std::transform(mFileExt.begin(), mFileExt.end(), mFileExt.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  try {
    mFileSize = fs::file_size(filepath);
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error getting file size: " << e.what() << std::endl;
  }

  mImageData =
      stbi_load(mImagePath.c_str(), &mImageWidth, &mImageHeight, &mChannels, 0);
  if (!mImageData) {
    std::cerr << "Error: " << stbi_failure_reason() << std::endl;
    return true;
  }
  if (mChannels < 3) {
    std::cerr << "Non-RGB image not supported" << std::endl;
    return true;
  }
  return false;
}

bool Image::save(const std::string &outputPath = "") {
  std::string savePath = outputPath;
  bool isSuccess = false;

  // idk
  if (savePath.empty()) {
    fs::path originalPath(mImagePath);
    savePath = originalPath.replace_extension(mFileExt).string();
  }

  if (mFileExt == ".png") {
    isSuccess =
        stbi_write_png(savePath.c_str(), mImageWidth, mImageHeight, mChannels,
                       mImageData, mImageWidth * mChannels) != 0;
  } else if (mFileExt == ".jpg" || mFileExt == ".jpeg") {

    isSuccess = stbi_write_jpg(savePath.c_str(), mImageWidth, mImageHeight,
                               mChannels, mImageData,
                               68 // quality
                               ) != 0;
  } else if (mFileExt == ".bmp") {
    isSuccess = stbi_write_bmp(savePath.c_str(), mImageWidth, mImageHeight,
                               mChannels, mImageData) != 0;
  } else if (mFileExt == ".tga") {
    isSuccess = stbi_write_tga(savePath.c_str(), mImageWidth, mImageHeight,
                               mChannels, mImageData);
  } else {

    savePath = outputPath.empty()
                   ? fs::path(mImagePath).replace_extension("png").string()
                   : outputPath;

    isSuccess =
        stbi_write_png(savePath.c_str(), mImageWidth, mImageHeight, mChannels,
                       mImageData, mImageWidth * mChannels) != 0;
  }
  if (isSuccess) {
    try {
      mFileSize = fs::file_size(savePath);
    } catch (const fs::filesystem_error &e) {
      std::cerr << "Error getting file size: " << e.what() << std::endl;
    }
  }
  return isSuccess;
}

static void count_bytes(void *context, void *data, int size) {
  (void)data;
  size_t *total = reinterpret_cast<size_t *>(context);
  *total += static_cast<size_t>(size);
}

long long Image::estimateFileSize() const {
  size_t totalBytes = 0;

  if (mFileExt == ".png") {
    stbi_write_png_to_func(count_bytes, &totalBytes, mImageWidth, mImageHeight,
                           mChannels, mImageData, mImageWidth * mChannels);
  } else if (mFileExt == ".jpg" || mFileExt == ".jpeg") {
    stbi_write_jpg_to_func(count_bytes, &totalBytes, mImageWidth, mImageHeight,
                           mChannels, mImageData, 68);
  } else if (mFileExt == ".bmp") {
    stbi_write_bmp_to_func(count_bytes, &totalBytes, mImageWidth, mImageHeight,
                           mChannels, mImageData);
  } else if (mFileExt == ".tga") {
    stbi_write_tga_to_func(count_bytes, &totalBytes, mImageWidth, mImageHeight,
                           mChannels, mImageData);
  } else {
    stbi_write_png_to_func(count_bytes, &totalBytes, mImageWidth, mImageHeight,
                           mChannels, mImageData, mImageWidth * mChannels);
  }
  return totalBytes;
}

void Image::computeSummedAreaTable() {
  // DEBUG_TIMER("Compute summed area table");
  int totalPixels = mImageWidth * mImageHeight;
  int tableSize = totalPixels * mChannels;
  mSummedAreaTable = new long long[tableSize];
  std::fill(mSummedAreaTable, mSummedAreaTable + tableSize, 0);

  for (int y = 0; y < mImageHeight; ++y) {
    for (int x = 0; x < mImageWidth; ++x) {
      int idx = y * (mImageWidth * mChannels) + x * mChannels;
      for (int c = 0; c < mChannels; ++c) {
        long long currentVal = static_cast<long long>(mImageData[idx + c]);
        long long left = (x > 0) ? mSummedAreaTable[idx - mChannels + c] : 0;
        long long above =
            (y > 0) ? mSummedAreaTable[idx - (mImageWidth * mChannels) + c] : 0;
        long long aboveLeft =
            (x > 0 && y > 0)
                ? mSummedAreaTable[idx - (mImageWidth * mChannels) - mChannels +
                                   c]
                : 0;
        mSummedAreaTable[idx + c] = currentVal + left + above - aboveLeft;
      }
    }
  }
}

void Image::computeSummedSquareTable() {
  // DEBUG_TIMER("Compute square area table");
  int totalPixels = mImageWidth * mImageHeight;
  int tableSize = totalPixels * mChannels;
  mSummedSquareTable = new long long[tableSize];
  std::fill(mSummedSquareTable, mSummedSquareTable + tableSize, 0);

  for (int y = 0; y < mImageHeight; ++y) {
    for (int x = 0; x < mImageWidth; ++x) {
      int idx = y * (mImageWidth * mChannels) + x * mChannels;
      for (int c = 0; c < mChannels; ++c) {
        long long currentVal = static_cast<long long>(mImageData[idx + c]);
        long long squareVal = currentVal * currentVal;
        long long left = (x > 0) ? mSummedSquareTable[idx - mChannels + c] : 0;
        long long above =
            (y > 0) ? mSummedSquareTable[idx - (mImageWidth * mChannels) + c]
                    : 0;
        long long aboveLeft =
            (x > 0 && y > 0)
                ? mSummedSquareTable[idx - (mImageWidth * mChannels) -
                                     mChannels + c]
                : 0;
        mSummedSquareTable[idx + c] = squareVal + left + above - aboveLeft;
      }
    }
  }
}

int Image::getIdxAt(int x, int y, int channel) const {
  return y * (mImageWidth * mChannels) + x * mChannels + channel;
}

std::array<unsigned char, 3> Image::getColorAt(int x, int y) const {
  std::array<unsigned char, 3> color = {0, 0, 0}; // RGB
  if (x < 0 || x >= mImageWidth || y < 0 || y >= mImageHeight)
    return color;

  color[0] = mImageData[getIdxAt(x, y, 0)];
  color[1] = mImageData[getIdxAt(x, y, 1)];
  color[2] = mImageData[getIdxAt(x, y, 2)];
  return color;
}

unsigned char Image::getAlphaAt(int x, int y) const {
  if (mChannels == 4) {
    return mImageData[getIdxAt(x, y, 3)];
  }
  return 255;
}

long long Image::getChannelBlockSum(int x, int y, int width, int height,
                                    int channel) const {

  long long A =
      (x > 0 && y > 0) ? mSummedAreaTable[getIdxAt(x - 1, y - 1, channel)] : 0;
  long long B =
      (y > 0) ? mSummedAreaTable[getIdxAt(x + width - 1, y - 1, channel)] : 0;
  long long C =
      (x > 0) ? mSummedAreaTable[getIdxAt(x - 1, y + height - 1, channel)] : 0;
  long long D =
      mSummedAreaTable[getIdxAt(x + width - 1, y + height - 1, channel)];

  return D - B - C + A;
}

long long Image::getChannelSquareBlockSum(int x, int y, int width, int height,
                                          int channel) const {
  long long A = (x > 0 && y > 0)
                    ? mSummedSquareTable[getIdxAt(x - 1, y - 1, channel)]
                    : 0;
  long long B =
      (y > 0) ? mSummedSquareTable[getIdxAt(x + width - 1, y - 1, channel)] : 0;
  long long C =
      (x > 0) ? mSummedSquareTable[getIdxAt(x - 1, y + height - 1, channel)]
              : 0;
  long long D =
      mSummedSquareTable[getIdxAt(x + width - 1, y + height - 1, channel)];
  return D - B - C + A;
}

void Image::setColorAt(int x, int y, unsigned char r, unsigned char g,
                       unsigned char b) {
  mImageData[getIdxAt(x, y, 0)] = r;
  mImageData[getIdxAt(x, y, 1)] = g;
  mImageData[getIdxAt(x, y, 2)] = b;
}

void Image::setAlphaAt(int x, int y, unsigned char a) {
  mImageData[getIdxAt(x, y, 3)] = a;
}

void Image::setBlockColorAt(int startX, int startY, int blockWidth,
                            int blockHeight, unsigned char r, unsigned char g,
                            unsigned char b) {
  if (!mImageData) {
    return;
  }

  int endX = std::min(startX + blockWidth, mImageWidth);
  int endY = std::min(startY + blockHeight, mImageHeight);
  if (startX < 0 || startY < 0 || startX >= mImageWidth ||
      startY >= mImageHeight)
    return;

  int effectiveBlockWidth = endX - startX;
  int rowSize = effectiveBlockWidth * mChannels;
  std::vector<unsigned char> rowBuffer(rowSize);

  for (int i = 0; i < effectiveBlockWidth; ++i) {
    rowBuffer[i * mChannels + 0] = r;
    rowBuffer[i * mChannels + 1] = g;
    rowBuffer[i * mChannels + 2] = b;
    if (mChannels == 4) {
      rowBuffer[i * mChannels + 3] = 255;
    }
  }

  for (int y = startY; y < endY; ++y) {
    unsigned char *rowPtr = mImageData + (y * mImageWidth + startX) * mChannels;
    std::memcpy(rowPtr, rowBuffer.data(), rowSize);
  }
}
