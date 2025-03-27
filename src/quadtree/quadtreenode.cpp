#include "quadtreenode.h"

QuadtreeNode::QuadtreeNode(int x, int y, int width, int height)
    : mPosX(x), mPosY(y), mWidth(width), mHeight(height), mIsDivided(false) {
  for (auto &child : mChildren)
    child = nullptr;
}

QuadtreeNode::~QuadtreeNode() {
  for (auto child : mChildren) {
    if (child != nullptr)
      delete child;
  }
}
