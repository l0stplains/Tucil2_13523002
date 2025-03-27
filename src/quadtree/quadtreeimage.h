#ifndef QUADTREEIMAGE_H
#define QUADTREEIMAGE_H

#include "image/image.h"
#include "quadtreenode.h"

enum class ErrorMethod {
  MEAN,
  SDV,
  SSIM,
};

class QuadtreeImage {
public:
  QuadtreeImage(const Image &image, float threshold, int minBlockSize,
                ErrorMethod errorMethod);
  ~QuadtreeImage();

  bool build();

  void clear();

  QuadtreeNode *getRoot() const { return mRoot; }

private:
  const Image &mImage;
  float mThreshold;
  int mMinBlockSize;
  ErrorMethod mErrorMethod;

  QuadtreeNode *mRoot;

  QuadtreeNode *buildNode(int x, int y, int width, int height);
};

#endif
