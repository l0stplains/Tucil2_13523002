#ifndef DEBUG_H
#define DEBUG_H

#include <chrono>
#include <iostream>
#include <string>

class DebugTimer {
public:
  DebugTimer(const std::string &name = "Block")
      : mName(name), mStart(std::chrono::high_resolution_clock::now()) {}

  ~DebugTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart)
            .count();
    std::cout << "[DEBUG] " << mName << " took " << duration << " ms."
              << std::endl;
  }

private:
  std::string mName;
  std::chrono::high_resolution_clock::time_point mStart;
};

#define DEBUG_TIMER(name) DebugTimer debugTimer_##__LINE__(name)

#endif
