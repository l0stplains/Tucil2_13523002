#ifndef ERROR_METHOD_H
#define ERROR_METHOD_H

#include "image/image.h"

class ErrorMethod {
public:
  virtual ~ErrorMethod() = default;
  virtual double calculateError(const Image &image, int x, int y, int width,
                                int height) const = 0;
  virtual bool isInErrorBound(double error) const = 0;
};

#endif
