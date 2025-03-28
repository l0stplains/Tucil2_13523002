#include "cli.h"
#include "error_measurement/emm_entropy.h"
#include "error_measurement/emm_mad.h"
#include "error_measurement/emm_mpd.h"
#include "error_measurement/emm_ssim.h"
#include "error_measurement/emm_variance.h"
#include "error_measurement/error_method.h"

#include "image/image.h"
#include "quadtree/quadtreeimage.h"
#include <iostream>
#include <string>

bool CLI::start() {
  std::string filename;
  std::cout << "Enter image file path: ";
  std::cin >> filename;

  Image userImage(filename);

  if (userImage.load()) {
    std::cerr << "Failed to load image: " << userImage.getImagePath() << "\n";
    return INPUT_FAILED;
  }

  std::string userErrorMethodChoice;
  ErrorMethod *errorMethod;
  std::cout << "Enter error method: ";
  std::cin >> userErrorMethodChoice;
  std::string &UEMC = userErrorMethodChoice;
  userImage.computeSummedAreaTable();
  if (UEMC == "MAD" || UEMC == "MeanAbsoluteDeviation") {
    errorMethod = new EMM::MeanAbsoluteDeviation();
  } else if (UEMC == "VAR" || UEMC == "Variance") {
    errorMethod = new EMM::Variance();
    userImage.computeSummedSquareTable();
  } else if (UEMC == "MPD" || UEMC == "MaximumPixelDifferece") {
    errorMethod = new EMM::MaximumPixelDifference();
  } else if (UEMC == "ENT" || UEMC == "Entropy") {
    errorMethod = new EMM::Entropy();
  } else if (UEMC == "SSIM" || UEMC == "StructuralSimilarityIndexMeasure") {
    errorMethod = new EMM::StructuralSimilarityIndexMeasure();
    userImage.computeSummedSquareTable();
  } else {
    std::cerr << "Error: Error Measurement Method unknown!\n";
    return INPUT_FAILED;
  }

  double thresholdInput;
  std::cout << "Enter threshold: ";
  std::cin >> thresholdInput;
  if (!errorMethod->isInErrorBound(thresholdInput)) {
    std::cerr << "Error: Threshold out of bound!\n";
    return INPUT_FAILED;
  }

  QuadtreeImage img(userImage, thresholdInput, 8 * 8, errorMethod);
  if (!img.build()) {
    std::cerr << "Error: Building Quadtree Failed!\n";
    return INPUT_FAILED;
  }

  std::cout << "Depth: " << img.getDepth()
            << " Node count: " << img.getNodeCount() << std::endl;

  std::string outputPath;
  std::cout << "Enter output file path: ";
  std::cin >> outputPath;

  Image resultImage = img.apply();
  resultImage.save(outputPath);

  ImageSequence resultSequence = img.applyAnimation();
  resultSequence.save(outputPath + ".gif");

  return INPUT_SUCCESS;
}
