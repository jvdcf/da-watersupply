#include "Runtime.h"
#include "Parser.h"
#include "Utils.h"
#include <cstdint>
#include <exception>
#include <iomanip>
#include <ios>
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
  std::cout
      << "Available commands:\n"
      << "  quit\n"
      << "      Quits this program.\n"
      << "  help\n"
      << "      Prints this help.\n"
      << "  count\n"
      << "      Number of cities, reservoirs and pumps. Useful for debug.\n"
      << "  maxFlowCity [city_id]\n"
      << "      Maximum amount of water that can reach each or a specific city.\n"
      << "  needsMet\n"
      << "      Cities with not enough flow for their demand.\n"
      << "  rm\n"
      << "      reservoir [reservoir_id]\n"
      << "          List the compromised cities if a reservoir, specific via the optional argument, can be removed, or, if empty, all that can be removed.\n"
      << "      pump [pump_id]\n"
      << "          List the compromised cities if a pump, specific via the optional argument, can be removed, or, if empty, all pumps that can be removed.\n"
      << "      pipe [<any_code> <any_code>]\n"
      << "          List the compromised cities if a pipe, specific via the optional arguments, can be removed, or, if empty, all pipes that can be removed.\n"
      << std::endl;
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
      error("City id '" + std::to_string(args[0].getInt().value()) +
            "' not found.");
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
    return;
  }
}

void Runtime::handleRmPump(std::vector<CommandLineValue> args) {
  if (args.empty()) {
    bool is_virgin = true;
    for (auto vx : data->getGraph().getVertexSet()) {
      if (vx->getInfo().getKind() == Info::Kind::Reservoir) {
        auto res = data->removeSite(vx);
        if (res.empty()) {
          is_virgin = false;
          std::cout << "If the pump " << vx->getInfo().getId()
                    << " is removed, no changes are observed" << std::endl;
        }
      }
    }
    if (is_virgin)
      warning("There is not any redundancy in the network!");
    return;
  }
  uint32_t id = args[0].getInt().value();
  auto tgt = Utils::findVertex(data->getGraph(), Info::Kind::Pump, id);
  if (tgt == nullptr)
    return;
  auto res = data->removeSite(tgt);
  if (res.empty()) {
    std::cout << "If the pump " << id << " is removed, no changes are observed"
              << std::endl;
    return;
  }
  std::cout << "The following cities would be affected:" << std::endl;
  for (auto r : res) {
    std::cout << "C_" << std::get<0>(r) << ": Old: " << std::get<1>(r)
              << ", New: " << std::get<2>(r) << std::endl;
  }
}

void Runtime::handleRmPipe(std::vector<CommandLineValue> args) {
  std::unordered_map<uint16_t, uint32_t> maxFlows = data->maxFlowCity();
  std::unordered_map<std::pair<std::string, std::string>,
                     std::unordered_map<uint16_t, uint32_t>, pair_hash>
      removingPipesImpact = data->removingPipes();

  if (removingPipesImpact.empty()) {
    error("No pipes found.\n");
    return;
  }
  if (args.size() == 2) {
    Vertex<Info> *vertexA;
    Vertex<Info> *vertexB;
    std::string codeA;
    std::string codeB;
    try {
      codeA = args[0].getStr().value();
      codeB = args[1].getStr().value();
      auto parsedA = Utils::parseCode(codeA);
      auto parsedB = Utils::parseCode(codeB);
      vertexA =
          Utils::findVertex(data->getGraph(), parsedA.first, parsedA.second);
      vertexB =
          Utils::findVertex(data->getGraph(), parsedB.first, parsedB.second);
    } catch (const std::exception &e) {
      error("Invalid codes");
      return;
    }

    std::pair<std::string, std::string> pipeId = std::make_pair(codeA, codeB);
    auto it = removingPipesImpact.find(pipeId);

    if (it == removingPipesImpact.end()) {
      info("The key " + codeA + " to " + codeB + " was not found.");
      auto edge = Utils::findEdge(vertexA, vertexB);
      if (edge != nullptr) {
        if (edge->getReverse() != nullptr) { // the edge is bidirectional
          info("Bidirectional edge found. Trying reverse order.");
          pipeId = std::make_pair(codeB, codeA);
          it = removingPipesImpact.find(pipeId);
        }
      } else
        error("Pipeline between " + codeA + " and " + codeB + " not found.");
    }
    if (it != removingPipesImpact.end()) {
      std::cout << "Impact of removing Pipeline from " << codeA << " to "
                << codeB << ":\n";
      std::cout << "City | Old Flow | New Flow | Difference\n";
      for (const auto &cityFlow : it->second) {
        auto cityId = cityFlow.first;
        auto newFlow = cityFlow.second;
        auto itCity = std::find_if(
            maxFlows.begin(), maxFlows.end(),
            [cityId](const auto &pair) { return pair.first == cityId; });
        int difference = newFlow - itCity->second;
        if (difference != 0)
          std::cout << std::setw(4)
                    << Utils::parseId(Info::Kind::City, cityFlow.first)
                    << std::setw(0) << " |" << std::setw(9) << itCity->second
                    << std::setw(0) << " |" << std::setw(9) << cityFlow.second
                    << std::setw(0) << " |" << std::setw(10) << std::showpos << difference
                    << std::endl;
      }
    }
  } else if (args.empty()) {
    int removablePipesCount = 0;
    std::cout << "Removable pipelines without impact:\n";
    for (const auto &[pipeId, cityFlows] : removingPipesImpact) {
      bool affectsFlow = false;
      for (const auto &cityFlow : cityFlows) {
        auto cityId = cityFlow.first;
        auto newFlow = cityFlow.second;

        // Find original flow
        unsigned int oldFlow = maxFlows[cityId];
        if (newFlow != oldFlow) {
          // at least one city was affected
          affectsFlow = true;
          break;
        }
      }
      if (!affectsFlow) {
        std::cout << pipeId.first << " to " << pipeId.second << '\n';
        ++removablePipesCount;
      }
    }
    std::cout << "Found " << removablePipesCount
              << " pipelines that won't affect the flow.\n";
  } else {
    std::cerr << "ERROR: Command 'removingPipes' requires either no "
                 "arguments or two arguments: [codeA] [codeB].\n";
  }
  return;
}

void Runtime::handleRmReservoir(std::vector<CommandLineValue> args) {
  if (args.empty()) {
    bool is_virgin = true;
    for (auto vx : data->getGraph().getVertexSet()) {
      if (vx->getInfo().getKind() == Info::Kind::Reservoir) {
        auto res = data->removeSite(vx);
        if (res.empty()) {
          is_virgin = false;
          std::cout << "If the reservoir " << vx->getInfo().getId()
            << " is removed, no changes are observed" << std::endl;
        }
      }
    }
    if (is_virgin)
      warning("There is not any redundancy in the network!");
    return;
  }
  uint32_t id = args[0].getInt().value();
  auto tgt = Utils::findVertex(data->getGraph(), Info::Kind::Reservoir, id);
  if (tgt == nullptr) 
    return;
  auto res = data->removeSite(tgt);
  if (res.empty()) {
    std::cout << "If the reservoir " << id
              << " is removed, no changes are observed" << std::endl;
    return;
  }
  std::cout << "The following cities would be affected:" << std::endl;
  for (auto r : res) {
    std::cout << "C_" << std::get<0>(r) << ": Old: " << std::get<1>(r)
              << ", New: " << std::get<2>(r) << std::endl;
  }
}

void Runtime::handleNeedsMet() {
  auto result = data->meetsWaterNeeds();
  if (result.empty())
    std::cout << "This network configuration meets the water needs of its "
                 "costumers.\n";
  else
    std::cout << "Cities with not enough flow for their demand:\n";
  for (const auto &pair : result) {
    std::cout << Utils::parseId(Info::Kind::City, pair.first.getId()) << ": "
              << (pair.second * (-1))
              << " (Flow: " << pair.first.getCap().value() - pair.second << '/'
              << pair.first.getCap().value() << ")\n";
  }
  return;
}

void Runtime::handleBalanceGraph() {
    auto result = data->balanceGraph();
    std::cout << "Average: " << std::get<0>(result.first) << " -> " << std::get<0>(result.second) << '\n';
    std::cout << "Variance: " << std::get<1>(result.first) << " -> " << std::get<1>(result.second) << '\n';
    std::cout << "Max difference:" << std::get<2>(result.first) << " -> " << std::get<2>(result.second) << '\n';
    return;
}

void Runtime::processArgs(std::string args) {
  POption<Command> cmd_res = parse_cmd()(args);
  if (!cmd_res.has_value())
    return error("The command '" + args +
                 "' is invalid. Type 'help' to know more.");
  auto [rest, cmd] = cmd_res.value();
  if (!rest.empty())
    warning("Trailing output: '" + rest + "'.");
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
  case Command::NeedsMet:
    return handleNeedsMet();
  case Command::RmPump:
    return handleRmPump(cmd.args);
  case Command::RmPipe:
    return handleRmPipe(cmd.args);
  case Command::Balance:
      return handleBalanceGraph();
  default:
    error("AAAAAAAAAAAAAAAAAAAAAAA");
    break;
  }

  info("Type 'help' to see the available commands.");
}
