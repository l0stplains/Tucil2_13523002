#ifndef QUADTREEIMAGE_H
#define QUADTREEIMAGE_H

#include "error_measurement/error_method.h"
#include "image/image.h"
#include "image/image_sequence.h"
#include "quadtreenode.h"

class QuadtreeImage {
private:
  const Image &mImage;
  float mThreshold;
  int mMinBlockSize;
  ErrorMethod *mErrorMethod;

  int mDepth;
  int mNodeCount;

  QuadtreeNode *mRoot;

  static constexpr int DEFAULT_SEQUENCE_DELAY = 70;

public:
  QuadtreeImage(const Image &image, float threshold, int minBlockSize,
                ErrorMethod *errorMethod);
  ~QuadtreeImage();

  bool build();

  Image apply();
  ImageSequence applyAnimation();

  void clear();

  int getDepth() const { return mDepth; }
  int getNodeCount() const { return mNodeCount; }
  QuadtreeNode *getRoot() const { return mRoot; }
};

#endif
