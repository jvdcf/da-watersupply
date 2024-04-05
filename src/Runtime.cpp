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
              << "    Number of cities, reservoirs and pumps. Useful for debug.\n"
              << "maxFlowCity [cityId]\n"
              << "    Maximum amount of water that can reach each or a specific city.\n"
              << "removingPumps [pumpId]\n"
              << "    Removable pump stations, which won't affect the flow of any city.\n"
              << "    Shows the impact on cities if the specified pump were to be removed.\n"
              << "removingPipes [srcCode] [destCode]\n"
              << "    Removable pipelines, which won't affect the flow of any city.\n"
              << "    Shows the impact on cities if the specified pipeline were to be removed.\n";


      return;
  }

  if (args[0] == "count") {
    std::array<int, 3> counts = data->countVertexes();
    std::cout << "Cities:     " << counts[0] << '\n'
              << "Reservoirs: " << counts[1] << '\n'
              << "Pumps:      " << counts[2] << '\n';
    return;
  }

  if (args[0] == "maxFlowCity") {
    std::vector<std::pair<uint16_t, uint32_t>> maxFlows = data->maxFlowCity();

    if (args.size() == 2) {
      uint16_t citySelected;
      try {citySelected = std::stoi(args[1]);}
      catch (const std::invalid_argument& e) { std::cerr << "ERROR: Invalid city id '" << args[1] << "'.\n"; return;}
      auto pair = std::find_if(maxFlows.begin(), maxFlows.end(),[citySelected](const std::pair<uint16_t, uint32_t>& p) {
        return p.first == citySelected;
      });
      if (pair == maxFlows.end()) std::cerr << "ERROR: City id '" << args[1] << "' not found.\n";
      else std::cout << Utils::parseId(Info::Kind::City, pair->first) << ": " << pair->second << '\n';

    } else if (args.size() == 1) {
      for (const auto &[cityId, flow]: maxFlows) {
        std::cout << Utils::parseId(Info::Kind::City, cityId) << ": " << flow << '\n';
      }

    } else std::cerr << "ERROR: Invalid number of arguments for 'maxFlowCity'.\n";
    return;
  }

    if (args[0] == "removingPumps") {
        std::vector<std::pair<uint16_t, uint32_t>> maxFlows = data->maxFlowCity();
        std::unordered_map<Info, std::vector<std::pair<uint16_t, uint32_t>>> newFlows = data->removingPumps();
        if (args.size() == 2) {
            uint16_t pumpSelected;
            try {
                pumpSelected = std::stoi(args[1]);
            } catch (const std::invalid_argument& e) {
                std::cerr << "ERROR: Invalid pump station id '" << args[1] << "'.\n";
                return;
            }

            // Find the pump station
            auto itPump = std::find_if(newFlows.begin(), newFlows.end(),
                                   [pumpSelected](const auto &pair) {
                                       return pair.first.getId() == pumpSelected;
                                   });

            if (itPump == newFlows.end()) {
                std::cerr << "ERROR: Pump station id '" << args[1] << "' not found.\n";
            } else {
                std::cout << "Impact of removing Pump Station " << Utils::parseId(Info::Kind::Pump, itPump->first.getId()) << ":\n";
                std::cout << "City | Old Flow | New Flow | Deficit\n";
                for (const auto &cityFlow : itPump->second) {
                    auto cityId = cityFlow.first;
                    auto newFlow = cityFlow.second;
                    auto itCity = std::find_if(maxFlows.begin(), maxFlows.end(),
                                           [cityId](const auto &pair)
                                           { return pair.first == cityId; });
                    unsigned int deficit = itCity->second - newFlow;
                    if (deficit > 0)
                        std::cout << Utils::parseId(Info::Kind::City, cityFlow.first) << "  |  "
                        << itCity->second << "         " << cityFlow.second << "        " << deficit << std::endl;
                }
            }
        } else {
            std::cout << "Removable pump stations without impact:\n";
            for (const auto &pumpImpact : newFlows) {
                bool affectsFlow = false;
                for (const auto &cityFlow : pumpImpact.second) {
                    auto cityId = cityFlow.first;
                    auto newFlow = cityFlow.second;

                    // Find original flow
                    auto itCity = std::find_if(maxFlows.begin(), maxFlows.end(),
                                               [cityId](const auto &pair) { return pair.first == cityId; });

                    if (itCity != maxFlows.end()) {
                        if (newFlow < itCity->second) {
                            // at least one city was affected
                            affectsFlow = true;
                            break;
                        }
                    }
                }

                if (!affectsFlow) {
                    std::cout << Utils::parseId(Info::Kind::Pump, pumpImpact.first.getId())  << std::endl;
                }
            }
        }
        return;
    }

    if (args[0] == "removingPipes") {
        auto removingPipesImpact = data->removingPipes();
        if (removingPipesImpact.empty()) {
            std::cout << "No pipes found.\n";
            return;
        }
        if (args.size() == 3) {
            std::pair<Info::Kind, uint32_t> parsedA = Utils::parseCode(args[1]);
            Vertex<Info> *vertexA = data->findVertex(parsedA.first, parsedA.second);
            std::string codeA = Utils::parseId(vertexA->getInfo().getKind(), vertexA->getInfo().getId());

            std::pair<Info::Kind, uint32_t> parsedB = Utils::parseCode(args[2]);
            Vertex<Info> *vertexB = data->findVertex(parsedB.first, parsedB.second);
            std::string codeB = Utils::parseId(vertexB->getInfo().getKind(), vertexB->getInfo().getId());

            std::pair<std::string, std::string> pipeId = std::make_pair(codeA, codeB);
            auto it = removingPipesImpact.find(pipeId);
            std::cout << "tried to find pipe id on removingpipesimpact" << std::endl;
            if (it == removingPipesImpact.end()) {
                auto edge = Data::findEdge(vertexA, vertexB);
                if (edge->getReverse() != nullptr) { // the edge is bidirectional
                    pipeId = std::make_pair(codeB, codeA);
                    it = removingPipesImpact.find(pipeId);
                }
                else std::cerr << "ERROR: Pipeline from " << codeA << " to " << codeB << " not found.\n";
            }
            if (it != removingPipesImpact.end()){
                std::cout << "Impact of removing Pipeline from " << codeA << " to " << codeB << ":\n";
                for (const auto &cityFlow : it->second) {
                    std::cout << "City " << Utils::parseId(Info::Kind::City, cityFlow.first) << " would have a water cityFlow of " << cityFlow.second << " units.\n";
                }
            }
        } else if (args.size() == 1) {
            int removablePipesCount = 0;
            std::cout << "Removable pipelines without impact:\n";
            for (const auto& [pipeId, cityFlows] : removingPipesImpact) {
                if (cityFlows.empty()) {
                    std::cout << pipeId.first << " to " << pipeId.second << '\n';
                    ++removablePipesCount;
                }
            }
            std::cout << "Found " << removablePipesCount << " removable pipelines.\n";
        } else {
            std::cerr << "ERROR: Command 'removingPipes' requires either no arguments or two arguments: [codeA] [codeB].\n";
        }
        return;
    }



    std::cerr << "ERROR: No such command '" << args[0] << "'.\n"
            << "Type 'help' to see the available commands.\n";
}
