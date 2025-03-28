#include "quadtreeimage.h"
#include "utils/debug.h"
#include <stack>

QuadtreeImage::QuadtreeImage(const Image &image, float threshold,
                             int minBlockSize, ErrorMethod *errorMethod)
    : mImage(image), mThreshold(threshold), mMinBlockSize(minBlockSize),
      mErrorMethod(errorMethod), mRoot(nullptr) {}

QuadtreeImage::~QuadtreeImage() { clear(); }

bool QuadtreeImage::build() {
  DEBUG_TIMER("Building tree");
  mRoot = new QuadtreeNode(0, 0, mImage.getWidth(), mImage.getHeight());
  std::stack<QuadtreeNode *> nodeStack;
  nodeStack.push(mRoot);

  while (!nodeStack.empty()) {
    QuadtreeNode *node = nodeStack.top();
    nodeStack.pop();

    double error = mErrorMethod->calculateError(
        mImage, node->mPosX, node->mPosY, node->mWidth, node->mHeight);

    if (error <= mThreshold) {
      continue;
    }

    if (node->mWidth * node->mHeight / 4 < mMinBlockSize) {
      continue;
    }

    if (!node->mIsDivided) {
      node->divide();
    }

    for (int i = 0; i < 4; ++i) {
      if (node->mChildren[i] != nullptr) {
        nodeStack.push(node->mChildren[i]);
      }
    }
  }

  return (mRoot != nullptr);
}

Image QuadtreeImage::apply() {
  DEBUG_TIMER("Applying tree to image");
  Image resultImage(mImage);

  std::stack<QuadtreeNode *> nodeStack;
  nodeStack.push(mRoot);

  while (!nodeStack.empty()) {
    QuadtreeNode *node = nodeStack.top();
    nodeStack.pop();

    if (!node->mIsDivided) {
      int area = node->mWidth * node->mHeight;

      unsigned char avgR = static_cast<unsigned char>(
          mImage.getChannelBlockSum(node->mPosX, node->mPosY, node->mWidth,
                                    node->mHeight, 0) /
          area);
      unsigned char avgG = static_cast<unsigned char>(
          mImage.getChannelBlockSum(node->mPosX, node->mPosY, node->mWidth,
                                    node->mHeight, 1) /
          area);
      unsigned char avgB = static_cast<unsigned char>(
          mImage.getChannelBlockSum(node->mPosX, node->mPosY, node->mWidth,
                                    node->mHeight, 2) /
          area);

      for (int x = node->mPosX; x < node->mPosX + node->mWidth; ++x) {
        for (int y = node->mPosY; y < node->mPosY + node->mHeight; ++y) {
          resultImage.setColorAt(x, y, avgR, avgG, avgB);
        }
      }

    } else {
      for (int i = 0; i < 4; ++i) {
        if (node->mChildren[i] != nullptr) {
          nodeStack.push(node->mChildren[i]);
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
