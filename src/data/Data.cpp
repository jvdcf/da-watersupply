#include "Data.h"
#include <utility>

// Constructor

Data::Data(Csv cities, Csv pipes, Csv reservoirs, Csv stations) {
  this->g = Graph<Info>();
  setCities(std::move(cities));
  setReservoirs(std::move(reservoirs));
  setStations(std::move(stations));
  setPipes(std::move(pipes));
}

void Data::setCities(Csv cities) {
  std::vector<CsvLine> data = cities.get_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      std::cerr << "WARN: Empty line in Cities.csv" << std::endl;
      continue;
    }

    std::string location = values[0].get_str();
    uint32_t id = values[1].get_int();
    float demand = values[3].get_flt();
    uint32_t population = values[4].get_int();

    const Info info = Info(
            Info::Kind::City,
            id,
            Info::CityData(demand, location, population));
    g.addVertex(info);
  }
}

void Data::setReservoirs(Csv reservoirs) {
  std::vector<CsvLine> data = reservoirs.get_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      std::cerr << "WARN: Empty line in Reservoir.csv" << std::endl;
      continue;
    }

    std::string reservoir = values[0].get_str();
    std::string municipality = values[1].get_str();
    uint32_t id = values[2].get_int();
    uint32_t capacity = values[4].get_int();

    const Info info = Info(
            Info::Kind::Reservoir,
            id,
            Info::ReservoirData(capacity, municipality, reservoir));
    g.addVertex(info);
  }
}

void Data::setStations(Csv stations) {
  std::vector<CsvLine> data = stations.get_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      std::cerr << "WARN: Empty line in Stations.csv" << std::endl;
      continue;
    }

    uint32_t id = values[0].get_int();

    const Info info = Info(
            Info::Kind::Pump,
            id,
            Info::PumpData());
    g.addVertex(info);
  }
}

void Data::setPipes(Csv pipes) {
  std::vector<CsvLine> data = pipes.get_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      std::cerr << "WARN: Empty line in Pipes.csv" << std::endl;
      continue;
    }

    std::string serviceA = values[0].get_str();
    std::string serviceB = values[1].get_str();
    uint32_t capacity = values[2].get_int();
    bool bidirectional = values[3].get_int();

    std::pair<Info::Kind, uint32_t> parsedA = Utils::parseCode(serviceA);
    std::pair<Info::Kind, uint32_t> parsedB = Utils::parseCode(serviceB);
    Vertex<Info> *vertexA = Utils::findVertex(g, parsedA.first, parsedA.second);
    Vertex<Info> *vertexB = Utils::findVertex(g, parsedB.first, parsedB.second);

    if (vertexA == nullptr || vertexB == nullptr) continue;

    if (bidirectional) g.addBidirectionalEdge(vertexA, vertexB, capacity);
    else g.addEdge(vertexA, vertexB, capacity);
  }
}

// Functions =================================================================================================

std::array<int, 3> Data::countVertexes() {
  std::array<int, 3> counts = {0, 0, 0};
  for (Vertex<Info> *v : g.getVertexSet()) {
    switch (v->getInfo().getKind()) {
      case Info::Kind::City:
        ++counts[0];
        break;
      case Info::Kind::Reservoir:
        ++counts[1];
        break;
      case Info::Kind::Pump:
        ++counts[2];
        break;
    }
  }
  return counts;
}

std::vector<std::pair<uint16_t, uint32_t>> Data::maxFlowCity() {
  Vertex<Info> *superSource = Utils::createSuperSource(&g);
  Vertex<Info> *superSink = Utils::createSuperSink(&g);
  Utils::EdmondsKarp(&g, superSource, superSink);

  std::vector<std::pair<uint16_t, uint32_t>> result; // por que nao um unordered_map?
  for (Vertex<Info> *v : g.getVertexSet()) {
    if (v->getInfo().getKind() == Info::Kind::City) {
      uint32_t flow = 0;
      for (Edge<Info> *e : v->getIncoming()) {
        flow += e->getFlow();
      }
      result.emplace_back(v->getInfo().getId(), flow);
    }
  }

  Utils::removeSuperSource(&g, superSource);
  Utils::removeSuperSink(&g, superSink);
  return result;
}

std::unordered_map<Info, std::vector<std::pair<uint16_t, int>>> Data::removablePumps() {
    std::vector<std::pair<uint16_t, uint32_t>> maxFlows = maxFlowCity();
    std::unordered_map<Info, std::vector<std::pair<uint16_t, int>>> pumpImpactMap;

    for (Vertex<Info> *v : g.getVertexSet()) {
        if (v->getInfo().getKind() == Info::Kind::Pump) {
            v->setActive(false);
            std::vector<std::pair<uint16_t, int>> cityDeficits;
            std::vector<std::pair<uint16_t, uint32_t>> newMaxFlows = maxFlowCity();
            for (const auto &[cityId, originalFlow] : maxFlows) {
                auto it = std::find_if(newMaxFlows.begin(), newMaxFlows.end(),
                                       [cityId](const auto &pair)
                                       { return pair.first == cityId; });
                if (it != newMaxFlows.end() && it->second < originalFlow) {
                    cityDeficits.emplace_back(cityId, originalFlow - it->second);
                }
            }
            pumpImpactMap[v->getInfo()] = cityDeficits;
            v->setActive(true);
        }
    }
    return pumpImpactMap;
}
//
//std::map<Info*, std::vector<std::pair<Info*, double>>> Data::evaluatePumpStationRemovalImpact() {
//    std::map<Info*, std::vector<std::pair<Info*, double>>> impactMap;
//    for (auto station : getPumpStations()) {
//        station->setActive(false);
//        auto deficits = calculateCityDeficits();
//        if (!deficits.empty()) {
//            impactMap[station->getInfo()] = deficits;
//        }
//        station->setActive(true);
//    }
//    return impactMap;
//}

