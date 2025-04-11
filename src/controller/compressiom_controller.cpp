#include "controller/compression_controller.h"
#include "quadtree/quadtreeimage.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <utility>

namespace fs = std::filesystem;

bool CompressionController::setInputPath(std::string path) {
  fs::path filePath(path);
  filePath = fs::absolute(filePath);
  std::string ext = filePath.extension().string();

  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (fs::exists(filePath)) {
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" ||
        ext == ".hdr" || ext == ".bmp") {
      mFileExt = ext;
      mInputPath = filePath.string();
      return true;
    }
  }
  return false;
}

bool CompressionController::setErrorMethod(ErrorMethod *method) {
  if (mErrorMethod != nullptr) {
    delete mErrorMethod;
    mErrorMethod = nullptr;
  }
  mErrorMethod = method;
  return true;
}

bool CompressionController::setThreshold(double threshold) {
  assert(mErrorMethod);
  if (mErrorMethod->isInErrorBound(threshold)) {
    mThreshold = threshold;
    return true;
  }
  return false;
}
bool CompressionController::setMinBlockSize(int blockSize) {
  mMinBlockSize = blockSize;
  return true;
}
bool CompressionController::setTargetCompression(double target) {
  if (target >= 0.0 && target <= 1.0) {
    mTargetCompression = target;
    return true;
  }
  return false;
}
bool CompressionController::setOutputPath(std::string path) {
  fs::path filePath(path);
  filePath = fs::absolute(filePath);
  std::string ext = filePath.extension().string();

  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (ext == mFileExt) {
    mOutputPath = filePath.string();
    return true;
  }
  return false;
}
bool CompressionController::setGifOutputPath(std::string path) {
  fs::path filePath(path);
  if (filePath.empty()) {
    mGifOutputPath = "";
    return true;
  }
  filePath = fs::absolute(filePath);
  std::string ext = filePath.extension().string();

  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (ext == ".gif") {
    mGifOutputPath = filePath.string();
    return true;
  }
  return false;
}
bool CompressionController::run(
    std::function<void(const ProgressStage &)> progressCallback) {

  progressCallback(ProgressStage::Loading);
  Image image(mInputPath);
  image.load();

  progressCallback(ProgressStage::Precompute);
  image.computeSummedAreaTable();
  std::string methodId = mErrorMethod->getIdentifier();
  if (methodId == "SIM" || methodId == "VAR") {
    image.computeSummedSquareTable();
  }

  if (mTargetCompression) {
    progressCallback(ProgressStage::FindingTarget);
    long long targetSize = (1.0 - mTargetCompression) * image.getFileSize();
    findTargetCompression(image, targetSize);
  }

  progressCallback(ProgressStage::BuildingTree);
  QuadtreeImage quadtree(image, mThreshold, mMinBlockSize, mErrorMethod);
  if (!quadtree.build()) {
    return false;
  }

  progressCallback(ProgressStage::TransformingImage);
  Image resultImage = quadtree.apply();

  progressCallback(ProgressStage::SavingImage);
  resultImage.save(mOutputPath);

  result.originalFileSize = image.getFileSize();
  result.compressedFileSize = resultImage.getFileSize();
  result.compressionPercentage =
      (1.0 -
       static_cast<double>(resultImage.getFileSize()) / image.getFileSize()) *
      100;
  result.quadtreeDepth = quadtree.getDepth();
  result.quadtreeNodeCount = quadtree.getNodeCount();
  result.outputFilePath = mOutputPath;
  result.gifOutputPath = mGifOutputPath;

  if (!mGifOutputPath.empty()) {
    progressCallback(ProgressStage::CreatingGif);
    ImageSequence resultSequence = quadtree.applyAnimation();
    resultSequence.save(mGifOutputPath);
  }

  progressCallback(ProgressStage::Finished);
  return true;
}

void CompressionController::findTargetCompression(Image &image,
                                                  long long targetSize) {
  long long leftSize, rightSize, middleSize;
  Image res(mInputPath);
  double rightThreshold = mErrorMethod->getUpperBound();
  double leftThreshold = mErrorMethod->getLowerBound();
  if (mErrorMethod->getIdentifier() == "SIM") {
    std::swap(rightThreshold, leftThreshold);
  }
  double middleThreshold;
  QuadtreeImage quadtreeLeft(image, leftThreshold, mMinBlockSize, mErrorMethod);
  quadtreeLeft.build();
  res = quadtreeLeft.apply();
  leftSize = res.estimateFileSize();
  if (targetSize > leftSize) {
    mThreshold = leftThreshold;
    return;
  }

  QuadtreeImage quadtreeRight(image, rightThreshold, mMinBlockSize,
                              mErrorMethod);
  quadtreeRight.build();
  res = quadtreeRight.apply();
  rightSize = res.estimateFileSize();

  if (targetSize < rightSize) {
    mThreshold = rightThreshold;
    return;
  }

  for (int i = 1; i < 32; i++) {
    middleThreshold = (leftThreshold + rightThreshold) / 2.0;
    if (std::abs(middleThreshold - rightThreshold) < 0.00001 &&
        std::abs(middleThreshold - leftThreshold) < 0.00001) {
      break;
    }
    QuadtreeImage quadtreeMiddle(image, middleThreshold, mMinBlockSize,
                                 mErrorMethod);
    quadtreeMiddle.build();
    res = quadtreeMiddle.apply();
    middleSize = res.estimateFileSize();

    if (std::abs(middleSize - targetSize) < 10) {
      mThreshold = middleThreshold;
      return;
    }
    if (targetSize > middleSize) {
      rightThreshold = middleThreshold;
    } else {
      leftThreshold = middleThreshold;
    }
  }
  mThreshold = middleThreshold;
  return;
}
