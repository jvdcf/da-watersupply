#include "Runtime.h"
#include <iostream>
#include <sstream>

Runtime::Runtime(Data *d) {
  this->data = d;
}

[[noreturn]] void Runtime::run() {
  std::cout << "Welcome to Water Supply Management.\n"
            << "Type 'help' to learn more.\n";

  std::vector<std::string> args;
  std::string input, buf;
  std::istringstream iss;

  while (true) {
    args.clear();
    input.clear();
    buf.clear();

    std::cout << "> ";
    getline(std::cin, input);
    iss = std::istringstream(input);
    while (std::getline(iss, buf, ' ')) {
      args.push_back(buf);
    }

    if(args.empty()) continue;
    processArgs(args);
  }
}

void Runtime::processArgs(const std::vector<std::string>& args) {
  if (args[0] == "quit") {
    std::cout << "Quitting...\n";
    exit(0);
  }

  if (args[0] == "help") {
    std::cout << "Available commands:\n"
              << "quit\n"
              << "    Quits this program.\n"
              << "help\n"
              << "    Prints this help.\n"
              << "count\n"
              << "    Number of cities, reservoirs and pumps. Useful for debug.\n";
    return;
  }

  if (args[0] == "count") {
    std::array<int, 3> counts = data->countVertexes();
    std::cout << "Cities:     " << counts[0] << '\n'
              << "Reservoirs: " << counts[1] << '\n'
              << "Pumps:      " << counts[2] << '\n';
    return;
  }

  std::cerr << "ERROR: No such command '" << args[0] << "'.\n"
            << "Type 'help' to see the available commands.\n";
}
