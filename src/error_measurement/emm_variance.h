#ifndef EMM_VARIANCE_H
#define EMM_VARIANCE_H

#include "error_method.h"
#include <cmath>
#include <cstdlib>

namespace EMM {
class Variance : public ErrorMethod {
private:
  static constexpr double kErrorUpperBound = 127.5 * 127.5;
  static constexpr double kErrorLowerBound = 0;

public:
  double calculateError(const Image &image, int x, int y, int width,
                        int height) const override {
    int count = width * height;

    long long sumR = image.getChannelBlockSum(x, y, width, height, 0);
    long long sumG = image.getChannelBlockSum(x, y, width, height, 1);
    long long sumB = image.getChannelBlockSum(x, y, width, height, 2);

    long long sumSqR = image.getChannelSquareBlockSum(x, y, width, height, 0);
    long long sumSqG = image.getChannelSquareBlockSum(x, y, width, height, 1);
    long long sumSqB = image.getChannelSquareBlockSum(x, y, width, height, 2);

    double meanR = static_cast<double>(sumR) / count;
    double meanG = static_cast<double>(sumG) / count;
    double meanB = static_cast<double>(sumB) / count;

    double varianceR = static_cast<double>(sumSqR) / count - meanR * meanR;
    double varianceG = static_cast<double>(sumSqG) / count - meanG * meanG;
    double varianceB = static_cast<double>(sumSqB) / count - meanB * meanB;

    return (varianceR + varianceG + varianceB) / 3;
  }
  inline bool isInErrorBound(double error) const override {
    return error <= kErrorUpperBound && error >= kErrorLowerBound;
  };
};
} // namespace EMM
#endif
