#include "quadtreeimage.h"
#include "utils/debug.h"
#include <queue>

QuadtreeImage::QuadtreeImage(const Image &image, float threshold,
                             int minBlockSize, ErrorMethod *errorMethod)
    : mImage(image), mThreshold(threshold), mMinBlockSize(minBlockSize),
      mErrorMethod(errorMethod), mDepth(0), mNodeCount(0), mRoot(nullptr) {}

QuadtreeImage::~QuadtreeImage() { clear(); }

bool QuadtreeImage::build() {
  DEBUG_TIMER("Building tree");
  mRoot = new QuadtreeNode(0, 0, mImage.getWidth(), mImage.getHeight());
  std::queue<QuadtreeNode *> nodeQueue;
  nodeQueue.push(mRoot);

  mNodeCount = 1;
  mDepth = 0;

  while (!nodeQueue.empty()) {
    const int nodesThisLevel = nodeQueue.size();

    for (int i = 0; i < nodesThisLevel; ++i) {
      QuadtreeNode *currentNode = nodeQueue.front();
      nodeQueue.pop();

      const double nodeError = mErrorMethod->calculateError(
          mImage, currentNode->mPosX, currentNode->mPosY, currentNode->mWidth,
          currentNode->mHeight);

      const bool exceedsErrorThreshold = nodeError > mThreshold;
      const bool hasMinimumSizeForDivision =
          (currentNode->mWidth * currentNode->mHeight) / 4 >= mMinBlockSize;

      if (exceedsErrorThreshold && hasMinimumSizeForDivision) {
        if (!currentNode->mIsDivided) {
          currentNode->divide();
        }

        for (const auto &child : currentNode->mChildren) {
          if (child) {
            ++mNodeCount;
            nodeQueue.push(child);
          }
        }
      }
    }

    if (!nodeQueue.empty()) {
      ++mDepth;
    }
  }

  return mRoot != nullptr;
}

Image QuadtreeImage::apply() {
  DEBUG_TIMER("Applying tree to image");
  Image resultImage(mImage);

  std::queue<QuadtreeNode *> nodeQueue;
  nodeQueue.push(mRoot);

  while (!nodeQueue.empty()) {
    QuadtreeNode *current = nodeQueue.front();
    nodeQueue.pop();

    if (!current->mIsDivided) {
      const int area = current->mWidth * current->mHeight;
      const int x = current->mPosX;
      const int y = current->mPosY;
      const int w = current->mWidth;
      const int h = current->mHeight;

      unsigned char avgR = static_cast<unsigned char>(
          mImage.getChannelBlockSum(x, y, w, h, 0) / area);
      unsigned char avgG = static_cast<unsigned char>(
          mImage.getChannelBlockSum(x, y, w, h, 1) / area);
      unsigned char avgB = static_cast<unsigned char>(
          mImage.getChannelBlockSum(x, y, w, h, 2) / area);

      for (int i = x; i < x + w; ++i) {
        for (int j = y; j < y + h; ++j) {
          resultImage.setColorAt(i, j, avgR, avgG, avgB);
        }
      }
    } else {
      for (auto &child : current->mChildren) {
        if (child != nullptr) {
          nodeQueue.push(child);
        }
      }
    }
  }

  return resultImage;
}

void QuadtreeImage::clear() {
  if (mRoot) {
    delete mRoot;
    mRoot = nullptr;
  }
}
