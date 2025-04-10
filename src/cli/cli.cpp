#include "cli.h"
#include "controller/compression_controller.h"
#include "error_measurement/emm_entropy.h"
#include "error_measurement/emm_mad.h"
#include "error_measurement/emm_mpd.h"
#include "error_measurement/emm_ssim.h"
#include "error_measurement/emm_variance.h"
#include "utils/style.h"
#include <algorithm>
#include <climits>
#include <cstddef>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include <condition_variable>
#include <string>

std::condition_variable spinnerCv;
std::mutex spinnerCvMutex;
namespace fs = std::filesystem;

const std::vector<std::string> CLI::SPINNER_FRAMES = {"⠋", "⠙", "⠹", "⠸", "⠼",
                                                      "⠴", "⠦", "⠧", "⠇", "⠏"};

CLI::CLI() : spinnerActive(false) {
  setupTerminal();
  state.currentInputStep = 0;
}

CLI::~CLI() {
  if (spinnerActive) {
    spinnerActive = false;
    if (spinnerThread && spinnerThread->joinable()) {
      spinnerThread->join();
    }
  }
  resetTerminal();
}

void CLI::setupTerminal() {
#ifdef _WIN32
  // windows terminal setup
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(hStdin, &oldConsoleMode);
  SetConsoleOutputCP(CP_UTF8);

  // enable virtual terminal processing for ANSI colors
  DWORD mode = 0;
  GetConsoleMode(hStdout, &mode);
  SetConsoleMode(hStdout, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

  DWORD inMode = 0;
  GetConsoleMode(hStdin, &inMode);
  // Set the mode to include processed input but disable Quick Edit if needed.
  SetConsoleMode(hStdin, ENABLE_PROCESSED_INPUT | ENABLE_EXTENDED_FLAGS |
                             (inMode & ~ENABLE_QUICK_EDIT_MODE));
#else
  // unix terminal setup
  struct termios newTermios;
  tcgetattr(STDIN_FILENO, &oldTermios);
  newTermios = oldTermios;
  newTermios.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
#endif
  // clear screen and show cursor
  std::cout << "\033[?25h" << std::flush;
}

void CLI::resetTerminal() {
#ifdef _WIN32
  SetConsoleMode(hStdin, oldConsoleMode);
#else
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
#endif
  // show cursor
  std::cout << "\033[?25h" << std::flush;
}

void CLI::clearScreen() { std::cout << FULL_CLEAR_SCREEN << std::flush; }
void CLI::clearVisibleScreen() { std::cout << CLEAR_SCREEN << std::flush; }

void CLI::clearLine() { std::cout << "\r\033[K" << std::flush; }

std::string CLI::doubleToString(double value, int precision = 2) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(precision) << value;
  return oss.str();
}

void CLI::moveCursorUp(int lines) {
  std::cout << "\033[" << lines << "A" << std::flush;
}

void CLI::moveCursorToColumn(int col) {
  std::cout << "\033[" << col << "G" << std::flush;
}

std::pair<int, int> CLI::getTerminalSize() {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hStdout, &csbi);
  return {csbi.srWindow.Right - csbi.srWindow.Left + 1,
          csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
#else
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return {w.ws_col, w.ws_row};
#endif
}

char CLI::getChar() {
#ifdef _WIN32
  return _getch();
#else
  char buf = 0;
  if (read(STDIN_FILENO, &buf, 1) < 0) {
    return -1;
  }
  return buf;
#endif
}

int CLI::getCharWithTimeout(int timeout_ms) {
#ifdef _WIN32
  // on Windows, we can use _kbhit() in a loop with a sleep to check for input
  auto start = std::chrono::steady_clock::now();
  while (!_kbhit()) {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
            .count() >= timeout_ms) {
      return -1; // timeout reached, no input available
    }
    Sleep(1); // sleep for 1 ms to reduce CPU usage
  }
  return _getch(); // input is available, return the character
#else
  fd_set set;
  FD_ZERO(&set);
  FD_SET(STDIN_FILENO, &set);

  struct timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;

  int rv = select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout);
  if (rv <= 0) {
    // timeout or error: no data available
    return -1;
  }
  // data is available, so use your getChar() function
  return getChar();
#endif
}

int CLI::handleArrowKeys() {
#ifdef _WIN32
<<<<<<< HEAD
	char c = getChar();
	switch (c) {
		case 72:
			return 1; // up
		case 80:
			return 2; // down
		case 75:
			return 3; // left
		case 77:
			return 4; // right
		default:
			return c;
	}
=======
  char c = getChar();
  std::cout << "TATANG: " << c << endl;
  if (c == 0 || c == 224) { // arrow keys prefix
    char d = getChar();
    std::cout << "TATANG: " << d << endl;
    switch (d) {
    case 72:
      return 1; // up
    case 80:
      return 2; // down
    case 75:
      return 3; // left
    case 77:
      return 4; // right
    default:
      return 0;
    }
  }
  return c;
>>>>>>> 09a94af891a59b73327923e81fa2aeac178f4381
#else
  char c = getChar();
  if (c == 27) { // ESC key detected
    int nextChar = getCharWithTimeout(50);
    if (nextChar == -1) {
      return 27;
    }
    if (nextChar == '[') {
      // Handle escape sequences for arrow keys or other keys
      switch (getChar()) {
      case 'A':
        return 1;
      case 'B':
        return 2;
      case 'D':
        return 3;
      case 'C':
        return 4;
      default:
        return 0;
      }
    }
    return 0;
  }
  return c;
#endif
}

std::string CLI::readInput(const std::string &placeholder) {
  std::string input;
  bool firstDraw = true;
  bool inputActive = true;
  std::cout << SHOW_CURSOR << BLOCK_CURSOR;
  std::cout << RESET << BRIGHT_BLACK << placeholder << RESET;
  moveCursorToColumn(5); // move back to the start position after '>'

  while (inputActive) {
    int c = handleArrowKeys();

    if (c == 27) { // ESC
      goToPreviousInput();
      return "BACK";
    } else if (c == 13 || c == 10) { // enter
      std::cout << std::endl;
      inputActive = false;
    } else if (c == 127 || c == 8) { // backspace
      if (!input.empty()) {
        input.pop_back();
        clearLine();
        std::cout << "  > " << input;
        if (input.empty() && !placeholder.empty()) {
          std::cout << BRIGHT_BLACK << placeholder << RESET;
          firstDraw = true;
          moveCursorToColumn(5);
        }
        std::cout << std::flush;
      }
    } else if (c >= 32 && c <= 126) { // printable ASCII
      // clear placeholder if this is the first character
      if (input.empty() && firstDraw && !placeholder.empty()) {
        clearLine();
        std::cout << "  > ";
        firstDraw = false;
      }
      input += static_cast<char>(c);
      std::cout << static_cast<char>(c) << std::flush;
    }
  }

  return input;
}

void CLI::printTitle(const std::string &title) {
  std::string paddedTitle = "  " + title + " ";

  std::cout << BOLD << paddedTitle << std::endl << std::endl;
}

void CLI::printHeader(const std::string &header) {
  std::cout << BOLD << header << RESET << std::endl << std::endl;
}

void CLI::printPrompt(const std::string &prompt, bool error) {
  std::cout << "  ";
  if (error) {
    std::cout << RED << ITALIC << "* " << RESET;
  }
  std::cout << prompt << std::endl;
}

void CLI::printHint(const std::string &hint) {
  clearLine();
  std::cout << BRIGHT_BLACK << "  " << hint << RESET << std::endl;
}

void CLI::printKeybind(std::vector<std::pair<std::string, std::string>> keys) {
  std::cout << "  ";
  for (int i = 0; (size_t)i < keys.size(); i++) {
    std::cout << BRIGHT_BLACK;
    if (i) {
      std::cout << " · ";
    }
    std::cout << RESET << DIM << keys[i].first << RESET << BRIGHT_BLACK << " "
              << keys[i].second << RESET;
  }
  std::cout << std::endl;
}

void CLI::printErrorMessage(const std::string &message) {
  clearLine();
  std::cout << std::endl;
  clearLine();
  std::cout << RED << ITALIC << "  " << message << RESET << std::endl;
}

void CLI::printPreviousInputs() {
  for (size_t i = 0; i < inputPromptHistory.size(); i++) {
    std::cout << RESET << "  " << inputPromptHistory[i] << RESET << std::endl;
  }
  std::cout << std::endl;
}

bool CLI::goToPreviousInput() {
  if (state.currentInputStep > 0) {
    state.currentInputStep--;
    if (!inputPromptHistory.empty()) {
      inputPromptHistory.pop_back();
    }
    return true;
  }
  return false;
}

std::string CLI::getFilePath(const std::string &prompt,
                             const std::string &description,
                             const std::string &placeholder, bool mustExist,
                             bool optional,
                             std::vector<std::string> fileExtensions) {
  bool isError = false;
  while (true) {
    clearLine();
    printPrompt(prompt, isError);
    std::vector<std::pair<std::string, std::string>> keybinds = {
        {"ENTER", "confirm"}};

    // im just lazy to differentiate file input and output
    if (!mustExist) {
      keybinds.push_back({"ESCAPE", "back"});
    }
    std::cout << "\n";
    clearLine();
    std::cout << "\n";
    clearLine();
    std::cout << "\n";
    clearLine();
    printKeybind(keybinds);
    moveCursorUp(4);

    printHint(description);

    clearLine();
    std::cout << "  > ";
    std::string path = readInput(placeholder);

    if (path.empty()) {
      if (optional) {
        return path;
      }
      moveCursorUp(4 + isError);
      isError = true;
      printErrorMessage("Path cannot be empty");
      continue;
    }

    if (path == "BACK") {
      std::cout << "\n";
      clearLine();
      std::cout << "\n";
      clearLine();

      moveCursorUp(4 + isError);
      if (mustExist)
        continue;
      return "BACK";
    }

    // check existence or directory validity
    if (mustExist) {
      if (!fs::exists(path)) {
        moveCursorUp(4 + isError);
        isError = true;
        printErrorMessage("File doesn't exist");
        continue;
      }
    } else {
      // for non-required files, check that the parent directory exists
      fs::path dirPath = fs::path(path).parent_path();
      if (!dirPath.empty() && !fs::exists(dirPath)) {
        moveCursorUp(4 + isError);
        isError = true;
        printErrorMessage("Output directory does not exist");
        continue;
      }
    }

    // check file extension if a list is provided
    if (!fileExtensions.empty()) {
      std::string ext = fs::path(path).extension().string();
      std::transform(ext.begin(), ext.end(), ext.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      bool validExt = false;
      for (const auto &allowedExt : fileExtensions) {
        std::string lowerAllowed = allowedExt;
        std::transform(lowerAllowed.begin(), lowerAllowed.end(),
                       lowerAllowed.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (ext == lowerAllowed) {
          validExt = true;
          break;
        }
      }
      if (!validExt) {
        std::string errMsg;
        if (fileExtensions.size() == 1) {
          errMsg = "File extension must be " + fileExtensions[0];
        } else {
          errMsg = "File extension must be ";
          for (size_t i = 0; i < fileExtensions.size(); i++) {
            errMsg += fileExtensions[i];
            if (i < fileExtensions.size() - 2) {
              errMsg += ", ";
            } else if (i == fileExtensions.size() - 2) {
              errMsg += " or ";
            }
          }
        }
        moveCursorUp(4 + isError);
        isError = true;
        printErrorMessage(errMsg);
        continue;
      }
    }

    return path;
  }
}

double CLI::getNumberInput(const std::string &prompt,
                           const std::string &description, double min,
                           double max, const std::string &placeholder) {
  bool isError = false;
  while (true) {
    clearLine();
    printPrompt(prompt, isError);
    std::vector<std::pair<std::string, std::string>> keybinds = {
        {"ENTER", "confirm"}, {"ESCAPE", "back"}};
    clearLine();
    std::cout << "\n";
    clearLine();
    std::cout << "\n";
    clearLine();
    std::cout << "\n";
    clearLine();
    printKeybind(keybinds);
    moveCursorUp(4);

    printHint(description);
    std::cout << "  > ";
    std::string input = readInput(placeholder);

    if (input == "BACK") {
      clearScreen();
      printTitle("QuadTree Image Compression");
      printPreviousInputs();
      return -1;
    }

    if (input.empty()) {
      moveCursorUp(4 + isError);
      isError = true;
      printErrorMessage("Input cannot be empty");
      continue;
    }

    try {
      double value = std::stod(input);
      if (value < min || value > max) {
        moveCursorUp(4 + isError);
        isError = true;
        std::ostringstream errorMsg;
        errorMsg << "Value must be between " << min << " and " << max;
        printErrorMessage(errorMsg.str());
        continue;
      }
      return value;
    } catch (const std::exception &) {
      moveCursorUp(4 + isError);
      isError = true;
      printErrorMessage("Invalid number format");
    }
  }
}

int CLI::getIntInput(const std::string &prompt, const std::string &description,
                     int min, int max, const std::string &placeholder) {
  bool isError = false;
  while (true) {
    clearLine();
    printPrompt(prompt, isError);
    std::vector<std::pair<std::string, std::string>> keybinds = {
        {"ENTER", "confirm"}, {"ESCAPE", "back"}};
    clearLine();
    std::cout << "\n";
    clearLine();
    std::cout << "\n";
    clearLine();
    std::cout << "\n";
    clearLine();
    printKeybind(keybinds);
    moveCursorUp(4);

    printHint(description);
    std::cout << "  > ";
    std::string input = readInput(placeholder);

    if (input == "BACK") {
      clearScreen();
      printTitle("QuadTree Image Compression");
      printPreviousInputs();
      return -1;
    }

    if (input.empty()) {
      moveCursorUp(4 + isError);
      isError = true;
      printErrorMessage("Input cannot be empty");
      continue;
    }

    try {
      int value = std::stoi(input);
      if (value < min || value > max) {
        moveCursorUp(4 + isError);
        isError = true;
        std::ostringstream errorMsg;
        if (max != INT_MAX) {
          errorMsg << "Value must be between " << min << " and " << max;
        } else {
          errorMsg << "Value must be at least 1";
        }
        printErrorMessage(errorMsg.str());
        continue;
      }
      return value;
    } catch (const std::exception &) {
      moveCursorUp(4 + isError);
      isError = true;
      printErrorMessage("Invalid number format");
    }
  }
}

template <typename T>
T CLI::selectOption(const std::string &prompt,
                    const std::vector<std::pair<std::string, T>> &options) {
  printPrompt(prompt, false);

  size_t selected = 0;
  bool selecting = true;

  while (selecting) {
    // print all options
    clearLine();
    for (size_t i = 0; i < options.size(); i++) {
      clearLine();
      if (i == selected) {
        std::cout << "  " << MAGENTA << "> " << RESET << BOLD
                  << options[i].second << ". " << options[i].first << RESET
                  << std::endl;
      } else {
        std::cout << "    " << options[i].second << ". " << options[i].first
                  << std::endl;
      }
    }

    std::vector<std::pair<std::string, std::string>> keybinds = {
        {"↑", "up"},
        {"↓", "down"},
        {"1-" + std::to_string((int)options.size()), "jump to"},
        {"ENTER", "confirm"},
        {"ESCAPE", "back"}};

    std::cout << "\n" << HIDE_CURSOR;
    printKeybind(keybinds);

    int key = handleArrowKeys();

    if (key == 1) {
      if (selected > 0)
        selected--;
      moveCursorUp(options.size() + 2);
    } else if (key == 2) {
      if (selected < options.size() - 1)
        selected++;
      moveCursorUp(options.size() + 2);
    } else if (key == 13 || key == 10) {
      selecting = false;
    } else if (key >= '1' && key <= '9') {
      int index = key - '1';
      if (index >= 0 && index < static_cast<int>(options.size())) {
        selected = index;
      }
      moveCursorUp(options.size() + 2);
    } else if (key == 27) {
      if (goToPreviousInput()) {
        clearScreen();
        printTitle("QuadTree Image Compression");
        printPreviousInputs();
        return 0;
      }
      moveCursorUp(options.size() + 2);
    } else {
      moveCursorUp(options.size() + 2);
    }
  }

  std::cout << RESET << std::endl;
  return options[selected].second;
}

void CLI::startSpinner(const std::string &message) {
  if (spinnerActive) {
    stopSpinner(true, "Previous operation completed");
  }

  spinnerActive = true;
  spinnerMessage = message;
  spinnerStartTime = std::chrono::steady_clock::now();

  spinnerThread = std::make_unique<std::thread>([this]() {
    int frame = 0;
    std::unique_lock<std::mutex> cvLock(spinnerCvMutex, std::defer_lock);
    while (spinnerActive) {
      std::string currentMessage;
      {
        std::lock_guard<std::mutex> lock(spinnerMutex);
        currentMessage = spinnerMessage;
      }

      auto now = std::chrono::steady_clock::now();
      double elapsedMs =
          std::chrono::duration<double, std::milli>(now - spinnerStartTime)
              .count();
      std::string timeStr;
      if (elapsedMs < 1000) {
        timeStr = doubleToString(elapsedMs) + " ms";
      } else {
        timeStr = doubleToString(elapsedMs / 1000.0).substr(0, 4) + " s";
      }

      clearLine();
      std::cout << "  " << CYAN << SPINNER_FRAMES[frame] << RESET << " "
                << currentMessage << " " << BRIGHT_BLACK << "(" << timeStr
                << ")" << RESET << std::flush;

      frame = (frame + 1) % SPINNER_FRAMES.size();

      // wait for 80ms or until notified
      cvLock.lock();
      spinnerCv.wait_for(cvLock, std::chrono::milliseconds(80),
                         [this]() { return !spinnerActive; });
      cvLock.unlock();
    }
  });
}

void CLI::stopSpinner(bool success, const std::string &message) {
  if (!spinnerActive)
    return;

  spinnerActive = false;
  // notify the spinner thread to wake up immediately
  spinnerCv.notify_all();
  if (spinnerThread && spinnerThread->joinable()) {
    spinnerThread->join();
  }

  // reasure elapsed time with high precision
  auto now = std::chrono::steady_clock::now();
  double elapsedMs =
      std::chrono::duration<double, std::milli>(now - spinnerStartTime).count();
  totalElapsedTime += elapsedMs;

  std::string timeStr;
  if (elapsedMs < 1000) {
    timeStr = doubleToString(elapsedMs) + " ms";
  } else {
    timeStr = doubleToString(elapsedMs / 1000.0).substr(0, 4) + " s";
  }

  clearLine();
  if (success) {
    std::cout << "  " << GREEN << "✓" << RESET << " " << message;
  } else {
    std::cout << "  " << RED << "✗" << RESET << " " << message;
  }
  std::cout << " " << BRIGHT_BLACK << "(" << timeStr << ")" << RESET
            << std::endl;
}

void CLI::updateSpinnerMessage(const std::string &message) {
  std::lock_guard<std::mutex> lock(spinnerMutex);
  spinnerMessage = message;
}
/*
void CLI::updateSpinnerProgress(double progress) {
  // if i even need, i could add progress bar functionality here
}
*/
CompressionResult CLI::processImage() {

  CompressionResult result;
  auto stageInfo =
      [](ProgressStage stage) -> std::pair<std::string, std::string> {
    switch (stage) {
    case ProgressStage::Loading:
      return {"Loading source image", "Source image loaded"};
    case ProgressStage::Precompute:
      return {"Precomputing summed area table", "Summed area table computed"};
    case ProgressStage::FindingTarget:
      return {"Finding target compression",
              "Target compression achieved with "};
    case ProgressStage::BuildingTree:
      return {"Building quadtree", "Building quadtree finished"};
    case ProgressStage::TransformingImage:
      return {"Compressing image", "Image compressed"};
    case ProgressStage::SavingImage:
      return {"Saving compressed image", "Compressed image saved"};
    case ProgressStage::CreatingGif:
      return {"Creating GIF", "GIF saved"};
    case ProgressStage::Finished:
      return {"Finishing", "Finished"};
    }
    return {"Processing", "Processing finished"};
  };

  ProgressStage lastStage = ProgressStage::Loading;
  bool firstCall = true;

  bool runResult = compression.run([&](const ProgressStage &stage) {
    auto [startMsg, stopMsg] = stageInfo(stage);

    if (!firstCall) {
      if (lastStage == ProgressStage::FindingTarget) {
        stopSpinner(true, stageInfo(lastStage).second +
                              doubleToString(compression.getThreshold()) +
                              " threshold");
      } else {

        stopSpinner(true, stageInfo(lastStage).second);
      }
    } else {
      firstCall = false;
    }

    if (stage != ProgressStage::Finished) {
      startSpinner(startMsg);
    }

    lastStage = stage;
  });

  if (runResult) {
    return compression.getResult();
  }
  return result;
}

CompressionResult CLI::run() {
  clearVisibleScreen();
  printTitle("QuadTree Image Compression");

  if (state.currentInputStep == 0) {
    std::string filePath =
        getFilePath("Enter input image file path",
                    "Supported file extension is .jpeg, .png, .bmp, .hdr, .tga",
                    "path/to/image.jpg", true, false,
                    {".jpeg", ".png", ".jpg", ".bmp", ".hdr", ".tga"});
    state.inputFilePath = filePath;
    compression.setInputPath(filePath);
    inputPromptHistory.push_back("Input image: " + state.inputFilePath);
    state.currentInputStep++;
  }

  if (state.currentInputStep == 1) {
    std::vector<std::pair<std::string, int>> errorMethods = {
        {"Variance", 1},
        {"Mean Absolute Deviation (MAD)", 2},
        {"Max Pixel Differece (MPD)", 3},
        {"Entropy", 4},
        {"Structural Similarity Index Measure (SSIM)", 5},
    };

    clearScreen();
    printTitle("QuadTree Image Compression");
    printPreviousInputs();

    int method =
        selectOption<int>("Select error measurement method", errorMethods);
    if (method == 0)
      return run();

    state.errorMethod = method;
    switch (method) {
    case 1:
      compression.setErrorMethod(new EMM::Variance());
      break;
    case 2:
      compression.setErrorMethod(new EMM::MeanAbsoluteDeviation());
      break;
    case 3:
      compression.setErrorMethod(new EMM::MaximumPixelDifference());
      break;
    case 4:
      compression.setErrorMethod(new EMM::Entropy());
      break;
    case 5:
      compression.setErrorMethod(new EMM::StructuralSimilarityIndexMeasure());
      break;
    }
    inputPromptHistory.push_back("Error method: " +
                                 errorMethods[method - 1].first);
    state.currentInputStep++;
  }

  if (state.currentInputStep == 2) {
    clearScreen();
    printTitle("QuadTree Image Compression");
    printPreviousInputs();

    double lower = compression.getErrorMethod()->getLowerBound();
    double upper = compression.getErrorMethod()->getUpperBound();
    double threshold =
        getNumberInput("Enter error threshold",
                       "Threshold is in range " + doubleToString(lower) + "-" +
                           doubleToString(upper),
                       lower, upper, "");
    if (threshold < 0)
      return run();

    state.threshold = threshold;
    compression.setThreshold(threshold);
    inputPromptHistory.push_back("Threshold: " + doubleToString(threshold));
    state.currentInputStep++;
  }

  if (state.currentInputStep == 3) {
    clearScreen();
    printTitle("QuadTree Image Compression");
    printPreviousInputs();

    int blockSize =
        getIntInput("Enter minimum block size (pixels²)", "", 1, INT_MAX, "");
    if (blockSize < 0)
      return run();

    state.minBlockSize = blockSize;
    compression.setMinBlockSize(blockSize);
    inputPromptHistory.push_back(
        "Min block size: " + std::to_string(blockSize) + " pixels²");
    state.currentInputStep++;
  }

  if (state.currentInputStep == 4) {
    clearScreen();
    printTitle("QuadTree Image Compression");
    printPreviousInputs();

    double target =
        getNumberInput("Enter compression target",
                       "0 for no target and 1.0 for 100%", 0.0, 1.0, "");
    if (target < 0)
      return run();

    state.compressionTarget = target;
    compression.setTargetCompression(target);
    if (target == 0) {
      inputPromptHistory.push_back("Compression target: None");
    } else {
      inputPromptHistory.push_back(
          "Compression target: " + doubleToString(target * 100) + "%");
    }
    state.currentInputStep++;
  }

  if (state.currentInputStep == 5) {
    clearScreen();
    printTitle("QuadTree Image Compression");
    printPreviousInputs();

    std::string outputPath = getFilePath(
        "Enter output file path", "Path could be absolute or relative",
        "path/to/output.png", false, false, {compression.getFileExt()});
    if (outputPath == "BACK")
      return run();

    state.outputFilePath = outputPath;
    if (!compression.setOutputPath(outputPath)) {
      std::cerr << "OUTPUTPATH WRONG " << std::endl;
    }
    inputPromptHistory.push_back("Output file: " + state.outputFilePath);
    state.currentInputStep++;
  }

  if (state.currentInputStep == 6) {
    clearScreen();
    printTitle("QuadTree Image Compression");
    printPreviousInputs();

    std::string gifPath = getFilePath(
        "Enter GIF visualization path (or leave empty to skip)",
        "Path could be absolute or relative",
        "path/to/visualization.gif (optional)", false, true, {".gif"});
    if (gifPath == "BACK")
      return run();

    state.gifOutputPath = gifPath;
    compression.setGifOutputPath(gifPath);
    if (gifPath.empty()) {
      inputPromptHistory.push_back("GIF visualization: None");
    } else {
      inputPromptHistory.push_back("GIF visualization: " + state.gifOutputPath);
    }
    state.currentInputStep++;
  }

  clearScreen();
  printTitle("QuadTree Image Compression");
  printPreviousInputs();

  std::cout << BOLD << "  Processing image..." << RESET << std::endl
            << std::endl;
  CompressionResult result = processImage();

  std::cout << std::endl;
  // clearScreen();
  printTitle("Compression Results");

  const int labelWidth = 25;
  auto formatSize = [](size_t size) -> std::string {
    std::ostringstream oss;
    if (size < 1024) {
      oss << size << " B";
    } else if (size < 1024 * 1024) {
      oss << std::fixed << std::setprecision(2) << (size / 1024.0) << " KB";
    } else {
      oss << std::fixed << std::setprecision(2) << (size / (1024.0 * 1024.0))
          << " MB";
    }
    return oss.str();
  };

  std::string timeStr;
  if (totalElapsedTime < 1000) {
    timeStr = doubleToString(totalElapsedTime) + " ms";
  } else {
    timeStr = doubleToString(totalElapsedTime / 1000.0).substr(0, 4) + " s";
  }

  std::cout << BOLD << "  Performance Metrics" << RESET << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Computation Time:" << timeStr << std::endl;

  std::cout << std::endl << BOLD << "  File Information" << RESET << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Original Size:" << formatSize(result.originalFileSize)
            << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Compressed Size:" << formatSize(result.compressedFileSize)
            << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Compression Ratio:" << std::fixed << std::setprecision(2)
            << result.compressionPercentage << "%" << std::endl;

  std::cout << std::endl
            << BOLD << "  Quadtree Statistics" << RESET << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Tree Depth:" << result.quadtreeDepth << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Node Count:" << result.quadtreeNodeCount << std::endl;

  if (compression.getTargetCompression()) {

    std::cout << "  " << std::left << std::setw(labelWidth)
              << "Target Threshold:"
              << std::to_string(compression.getThreshold()) << std::endl;
  }

  std::cout << std::endl << BOLD << "  Output Files" << RESET << std::endl;
  std::cout << "  " << std::left << std::setw(labelWidth)
            << "Compressed File:" << result.outputFilePath << std::endl;

  if (!result.gifOutputPath.empty()) {
    std::cout << "  " << std::left << std::setw(labelWidth)
              << "Visualization GIF:" << result.gifOutputPath << std::endl;
  }

  std::cout << std::endl;
  std::cout << GREEN << "  Compression completed successfully!" << RESET
            << std::endl;
  std::cout << std::endl;
  std::cout << "  Press any key to exit..." << std::flush;
  getChar();

  clearVisibleScreen();
  return result;
}
