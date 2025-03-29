#ifndef EMM_ENTROPY_H
#define EMM_ENTROPY_H

#include "error_method.h"
#include <array>
#include <cmath>

namespace EMM {

class Entropy : public ErrorMethod {
private:
  // 8-bit channel: log2(256) = 8.
  static constexpr double kErrorUpperBound = 8.0;
  static constexpr double kErrorLowerBound = 0.0;

public:
  double calculateError(const Image &image, int x, int y, int width,
                        int height) const override {
    int count = width * height;
    std::array<int, 256> histR = {0};
    std::array<int, 256> histG = {0};
    std::array<int, 256> histB = {0};

    for (int i = x; i < x + width; ++i) {
      for (int j = y; j < y + height; ++j) {
        std::array<unsigned char, 3> color = image.getColorAt(i, j);
        ++histR[color[0]];
        ++histG[color[1]];
        ++histB[color[2]];
      }
    }

    double entropyR = 0.0, entropyG = 0.0, entropyB = 0.0;
    for (int i = 0; i < 256; ++i) {
      if (histR[i] > 0) {
        double p = static_cast<double>(histR[i]) / count;
        entropyR -= p * std::log2(p);
      }
      if (histG[i] > 0) {
        double p = static_cast<double>(histG[i]) / count;
        entropyG -= p * std::log2(p);
      }
      if (histB[i] > 0) {
        double p = static_cast<double>(histB[i]) / count;
        entropyB -= p * std::log2(p);
      }
    }
    return (entropyR + entropyG + entropyB) / 3.0;
  }

  inline bool isInErrorBound(double error) const override {
    return error <= kErrorUpperBound && error >= kErrorLowerBound;
  }
};

} // namespace EMM

#endif
