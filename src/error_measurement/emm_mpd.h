#ifndef EMM_MPD_H
#define EMM_MPD_H

#include "error_method.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>

namespace EMM {
class MaximumPixelDifference : public ErrorMethod {
private:
  static constexpr double kErrorUpperBound = 255;
  static constexpr double kErrorLowerBound = 0;

public:
  double calculateError(const Image &image, int x, int y, int width,
                        int height) const override {
    std::array<unsigned char, 3> firstColor = image.getColorAt(x, y);
    unsigned char maxR = firstColor[0];
    unsigned char maxG = firstColor[1];
    unsigned char maxB = firstColor[2];

    unsigned char minR = firstColor[0];
    unsigned char minG = firstColor[1];
    unsigned char minB = firstColor[2];

    for (int i = x; i < x + width; ++i) {
      for (int j = y; j < y + height; ++j) {
        std::array<unsigned char, 3> color = image.getColorAt(i, j);
        maxR = std::max(maxR, color[0]);
        maxG = std::max(maxG, color[1]);
        maxB = std::max(maxB, color[2]);

        minR = std::min(minR, color[0]);
        minG = std::min(minG, color[1]);
        minB = std::min(minB, color[2]);

        // early exit if maximum difference is reached for all channels
        if (maxR == 255 && minR == 0 && maxG == 255 && minG == 0 &&
            maxB == 255 && minB == 0) {
          return 255.0;
        }
      }
    }

    return static_cast<double>(maxR - minR + maxG - minG + maxB - minB) / 3;
  }
  inline bool isInErrorBound(double error) const override {
    return error <= kErrorUpperBound && error >= kErrorLowerBound;
  };
};
} // namespace EMM
#endif
