#pragma once

#include "controller/compression_controller.h"
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "utils/style.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

class CLI {
public:
  CLI();
  ~CLI();

  // Main method to run the CLI interface
  CompressionResult run();

private:
  CompressionController compression;

  std::chrono::steady_clock::time_point spinnerStartTime =
      std::chrono::steady_clock::now();
  double totalElapsedTime = 0;

  // Terminal handling and utilities
  void setupTerminal();
  void resetTerminal();
  void clearScreen();
  void clearVisibleScreen();
  void clearLine();
  std::string doubleToString(double value, int precision);
  void moveCursorUp(int lines);
  void moveCursorToColumn(int col);
  std::pair<int, int> getTerminalSize();

  // Input handling
  char getChar();
  int getCharWithTimeout(int);
  std::string readInput(const std::string &placeholder = "");
  int handleArrowKeys();

  // Text formatting and display
  void printTitle(const std::string &title);
  void printHeader(const std::string &header);
  void printPrompt(const std::string &prompt, bool error = false);
  void printHint(const std::string &hint);
  void printKeybind(const std::vector<std::pair<std::string, std::string>>);
  void printHelp();
  void printErrorMessage(const std::string &message);
  void printPreviousInputs();

  // Selection handling
  template <typename T>
  T selectOption(const std::string &prompt,
                 const std::vector<std::pair<std::string, T>> &options);

  // Input gathering methods
  std::string getFilePath(const std::string &prompt,
                          const std::string &description,
                          const std::string &placeholder, bool mustExist,
                          bool optional,
                          std::vector<std::string> fileExtensions);
  double getNumberInput(const std::string &prompt,
                        const std::string &description, double min, double max,
                        const std::string &placeholder);
  int getIntInput(const std::string &prompt, const std::string &description,
                  int min, int max, const std::string &placeholder);

  // Progress indicators
  void startSpinner(const std::string &message);
  void updateSpinnerMessage(const std::string &message);
  void updateSpinnerProgress(double progress);
  void stopSpinner(bool success, const std::string &message);

  // Process execution
  CompressionResult processImage();

  // Navigation
  bool goToPreviousInput();
  void showHelpMenu();

  // State variables
  struct InputState {
    std::string inputFilePath;
    int errorMethod;
    double threshold;
    int minBlockSize;
    double compressionTarget;
    std::string outputFilePath;
    std::string gifOutputPath;
    int currentInputStep;
  };

  InputState state;
  std::vector<std::string> inputPromptHistory;

  // Spinner related
  std::atomic<bool> spinnerActive;
  std::unique_ptr<std::thread> spinnerThread;
  std::mutex spinnerMutex;
  std::string spinnerMessage;

  // Terminal state
#ifdef _WIN32
  HANDLE hStdin;
  HANDLE hStdout;
  DWORD oldConsoleMode;
#else
  struct termios oldTermios;
#endif

  // Braille spinner frames
  static const std::vector<std::string> SPINNER_FRAMES;
};
