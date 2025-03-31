#ifndef COMPRESSION_CONTROLLER_H
#define COMPRESSION_CONTROLLER_H

#include "error_measurement/error_method.h"
#include <functional>
#include <string>

enum class ProgressStage {
  Loading,
  Precompute,
  FindingTarget,
  BuildingTree,
  TransformingImage,
  SavingImage,
  CreatingGif,
  Finished
};

struct CompressionResult {
  double computationTime;
  size_t originalFileSize;
  size_t compressedFileSize;
  double compressionPercentage;
  int quadtreeDepth;
  int quadtreeNodeCount;
  std::string outputFilePath;
  std::string gifOutputPath;
};

class CompressionController {
private:
  std::string mInputPath;
  ErrorMethod *mErrorMethod;
  double mThreshold;
  int mMinBlockSize;
  double mTargetCompression;
  std::string mOutputPath;
  std::string mGifOutputPath;

  std::string mFileExt;

  CompressionResult result;

public:
  CompressionController() : mErrorMethod(nullptr){};

  std::string getInputPath() const { return mInputPath; }
  ErrorMethod *getErrorMethod() const { return mErrorMethod; }
  double getThreshold() const { return mThreshold; }
  int getMinBlockSize() const { return mMinBlockSize; }
  double getTargetCompression() const { return mTargetCompression; }
  std::string getOutputPath() const { return mOutputPath; }
  std::string getGifOutputPath() const { return mGifOutputPath; }
  std::string getFileExt() const { return mFileExt; }
  CompressionResult getResult() const { return result; }

  bool setInputPath(std::string);
  bool setErrorMethod(ErrorMethod *);
  bool setThreshold(double);
  bool setMinBlockSize(int);
  bool setTargetCompression(double);
  bool setOutputPath(std::string);
  bool setGifOutputPath(std::string);

  bool run(std::function<void(const ProgressStage &)>);
};

#endif
