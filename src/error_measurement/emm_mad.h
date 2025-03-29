#ifndef EMM_MAD_H
#define EMM_MAD_H

#include "error_method.h"
#include <array>
#include <cmath>
#include <cstdlib>

namespace EMM {
class MeanAbsoluteDeviation : public ErrorMethod {
private:
  static constexpr double kErrorUpperBound = 127.5;
  static constexpr double kErrorLowerBound = 0;

public:
  double calculateError(const Image &image, int x, int y, int width,
                        int height) const override {
    std::array<unsigned int, 3> sum = {0, 0, 0};
    int count = width * height;

    double avgR =
        static_cast<double>(image.getChannelBlockSum(x, y, width, height, 0)) /
        count;
    double avgG =
        static_cast<double>(image.getChannelBlockSum(x, y, width, height, 1)) /
        count;
    double avgB =
        static_cast<double>(image.getChannelBlockSum(x, y, width, height, 2)) /
        count;

    for (int i = y; i < y + height; ++i) {
      for (int j = x; j < x + width; ++j) {
        std::array<unsigned char, 3> color = image.getColorAt(j, i);
        sum[0] += std::abs(color[0] - avgR);
        sum[1] += std::abs(color[1] - avgG);
        sum[2] += std::abs(color[2] - avgB);
      }
    }

    double madR = static_cast<double>(sum[0]) / count;
    double madG = static_cast<double>(sum[1]) / count;
    double madB = static_cast<double>(sum[2]) / count;

    return (madR + madG + madB) / 3;
  }

  inline bool isInErrorBound(double error) const override {
    return error <= kErrorUpperBound && error >= kErrorLowerBound;
  };

  bool getUpperBound() const override { return kErrorUpperBound; }
  bool getLowerBound() const override { return kErrorLowerBound; }

  bool isQualityAcceptable(double mad, double threshold) const override {
    return mad <= threshold;
  }
};
} // namespace EMM
#endif
