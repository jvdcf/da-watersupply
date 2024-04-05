#include "Runtime.h"
#include "Parser.h"
#include "Utils.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

Runtime::Runtime(Data *d) { this->data = d; }

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
    if (input.empty())
      continue;
    processArgs(input);
  }
}

void Runtime::printHelp() {
  std::cout << "Available commands:\n"
            << "quit\n"
            << "    Quits this program.\n"
            << "help\n"
            << "    Prints this help.\n"
            << "count\n"
            << "    Number of cities, reservoirs and pumps. Useful for debug.\n"
            << "maxFlowCity [cityId]\n"
            << "    Maximum amount of water that can reach each or a specific "
               "city.\n";
}

void Runtime::handleQuit() {
  info("Quitting...");
  exit(0);
}

void Runtime::handleCount() {
  std::array<int, 3> counts = data->countVertexes();
  std::cout << "Cities:     " << counts[0] << '\n'
            << "Reservoirs: " << counts[1] << '\n'
            << "Pumps:      " << counts[2] << '\n';
}

void Runtime::handleMaxFlowCity(std::vector<CommandLineValue> args) {
  std::unordered_map<uint16_t, uint32_t> maxFlows = data->maxFlowCity();
  if (!args.empty()) {
    if (!args[0].getInt().has_value()) {
      error("Invalid input in maxFlowCity");
    }
    uint16_t citySelected = args[0].getInt().value();
    if (!maxFlows.contains(citySelected)) {
      error("City id '" + std::to_string(args[0].getInt().value()) + "' not found.");
      return;
    }
    std::cout << Utils::parseId(Info::Kind::City, citySelected) << ": "
              << maxFlows.at(citySelected) << '\n';
  } else {
    uint32_t sumMaxFlow = 0;
    for (auto &maxFlow : maxFlows) {
      std::cout << Utils::parseId(Info::Kind::City, maxFlow.first) << ": "
                << maxFlow.second << '\n';
      sumMaxFlow += maxFlow.second;
    }
    std::cout << "Max flow of the network: " << sumMaxFlow << '\n';
  }
}

void Runtime::handleRmReservoir(std::vector<CommandLineValue> args) {
  uint32_t id = args[0].getInt().value();
  if (Utils::findVertex(data->getGraph(), Info::Kind::Reservoir, id) == nullptr) return;
  auto res = data->removeReservoir(id);
  if (res.empty()) {
    std::cout << "If the reservoir " << id << " is removed, no changes are observed" << std::endl;
    return;
  }
  std::cout << "The following cities would be affected:" << std::endl;
  for (auto r : res) {
    std::cout << "C_" << std::get<0>(r) << ": Old: " << std::get<1>(r) << ", New: " << std::get<2>(r) << std::endl;
  }
}

void Runtime::processArgs(std::string args) {
  POption<Command> cmd_res = parse_cmd()(args);
  if (!cmd_res.has_value())
    return error("The command '" + args +
                 "' is invalid. Type 'help' to know more.");
  auto [rest, cmd] = cmd_res.value();
  if (!rest.empty()) warning("Trailing output: '" + rest + "'.");
  switch (cmd.command) {
  case Command::Help:
    return printHelp();
  case Command::Quit:
    return handleQuit();
  case Command::Count:
    return handleCount();
  case Command::MaxFlowCity:
    return handleMaxFlowCity(cmd.args);
  case Command::RmReservoir:
    return handleRmReservoir(cmd.args);
  case Command::RmPump:
  case Command::RmPipe:
    error("AAAAAAAAAAAAAAAAAAAAAAA");
    break;
  }

  info("Type 'help' to see the available commands.");
}
