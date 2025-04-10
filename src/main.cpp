#include "cli/cli.h"
#include "utils/style.h"
#include <csignal>
#include <cstdlib>

// handle SIGINT
void sigintHandler(int signum) {
  std::cout << CLEAR_SCREEN << "Interrupt received. Exiting...\n";
  std::exit(signum);
}

int main() {
  std::signal(SIGINT, sigintHandler);
  CLI cli;
  cli.run();
  return 0;
}
