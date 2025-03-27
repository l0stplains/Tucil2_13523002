#include "quadtreeimage.h"

QuadtreeImage::QuadtreeImage(const Image &image, float threshold,
                             int minBlockSize, ErrorMethod errorMethod)
    : mImage(image), mThreshold(threshold), mMinBlockSize(minBlockSize),
      mErrorMethod(errorMethod), mRoot(nullptr) {}

QuadtreeImage::~QuadtreeImage() { clear(); }

bool QuadtreeImage::build() {
  mRoot = buildNode(0, 0, mImage.getWidth(), mImage.getHeight());
  return (mRoot != nullptr);
}

void QuadtreeImage::clear() {
  if (mRoot) {
    delete mRoot;
    mRoot = nullptr;
  }
}

QuadtreeNode *QuadtreeImage::buildNode(int x, int y, int width, int height) {
  // TODO: Error Measurement

  if (width * height <= mMinBlockSize) {
    return new QuadtreeNode(x, y, width, height);
  }

  QuadtreeNode *node = new QuadtreeNode(x, y, width, height);
  node->mIsDivided = true;
  node->divide();

  return node;
}

