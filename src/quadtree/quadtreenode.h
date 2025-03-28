#ifndef QUADTREENODE_H
#define QUADTREENODE_H

#include <cassert>

class QuadtreeNode {
public:
  int mPosX;
  int mPosY;

  int mWidth;
  int mHeight;

  bool mIsDivided;

  QuadtreeNode *mChildren[4];

  QuadtreeNode(int x, int y, int width, int height);
  ~QuadtreeNode();

  QuadtreeNode(const QuadtreeNode &) = delete;
  QuadtreeNode &operator=(const QuadtreeNode &) = delete;

  inline void divide() {
    if (mIsDivided) {
      return;
    }

    int halfWidth = mWidth / 2;
    int halfHeight = mHeight / 2;
    // first quadrant
    mChildren[0] = new QuadtreeNode(mPosX + halfWidth, mPosY,
                                    mWidth - halfWidth, halfHeight);
    // second quadrant
    mChildren[1] = new QuadtreeNode(mPosX, mPosY, halfWidth, halfHeight);
    // third quadran
    mChildren[2] = new QuadtreeNode(mPosX, mPosY + halfHeight, halfWidth,
                                    mHeight - halfHeight);
    // fourth quadran
    mChildren[3] = new QuadtreeNode(mPosX + halfWidth, mPosY + halfHeight,
                                    mWidth - halfWidth, mHeight - halfHeight);
    mIsDivided = true;
  }
};

#endif
