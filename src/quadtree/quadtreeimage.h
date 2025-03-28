#ifndef QUADTREEIMAGE_H
#define QUADTREEIMAGE_H

#include "error_measurement/error_method.h"
#include "image/image.h"
#include "quadtreenode.h"

class QuadtreeImage {
private:
  const Image &mImage;
  float mThreshold;
  int mMinBlockSize;
  ErrorMethod *mErrorMethod;

  QuadtreeNode *mRoot;

  void buildNode(QuadtreeNode *node);
  void transformNode(QuadtreeNode *node, Image &image);

public:
  QuadtreeImage(const Image &image, float threshold, int minBlockSize,
                ErrorMethod *errorMethod);
  ~QuadtreeImage();

  bool build();

  Image transform();

  void clear();

  QuadtreeNode *getRoot() const { return mRoot; }
};

#endif
