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
    bool unidirectional = values[3].get_int();

    std::pair<Info::Kind, uint32_t> parsedA = Utils::parseCode(serviceA);
    std::pair<Info::Kind, uint32_t> parsedB = Utils::parseCode(serviceB);
    Vertex<Info> *vertexA = Utils::findVertex(g, parsedA.first, parsedA.second);
    Vertex<Info> *vertexB = Utils::findVertex(g, parsedB.first, parsedB.second);

    if (vertexA == nullptr || vertexB == nullptr) continue;

    if (!unidirectional) g.addBidirectionalEdge(vertexA, vertexB, capacity);
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

  std::vector<std::pair<uint16_t, uint32_t>> result;
  for (Vertex<Info> *v : g.getVertexSet()) {
    if (v->getInfo().getKind() == Info::Kind::City) {
      uint32_t flow = 0;
      for (Edge<Info> *e : v->getIncoming()) {
          // if flow + e->getFlow() <= e->getCapacity(), flow+=e->getFlow()
        flow += e->getFlow();
      }
      result.emplace_back(v->getInfo().getId(), flow);
    }
  }

  Utils::removeSuperSource(&g, superSource);
  Utils::removeSuperSink(&g, superSink);
  return result;
}

std::unordered_map<Info, std::vector<std::pair<uint16_t, uint32_t>>> Data::removingPumps() {
    std::unordered_map<Info, std::vector<std::pair<uint16_t, uint32_t>>> pumpImpactMap;
    for (Vertex<Info> *v : g.getVertexSet()) {
        if (v->getInfo().getKind() == Info::Kind::Pump) {
            v->setActive(false);
            std::vector<std::pair<uint16_t, uint32_t>> newFlows = maxFlowCity();
            pumpImpactMap[v->getInfo()] = newFlows;
            v->setActive(true);
        }
    }
    return pumpImpactMap;
}

Vertex<Info>* Data::findVertex(Info::Kind kind, uint32_t id){
    return Utils::findVertex(g, kind, id);
}

Edge<Info>* Data::findEdge(Vertex<Info>* vertexA, Vertex<Info>* vertexB) {
    if (!vertexA || !vertexB) {
        throw std::runtime_error("Source or destination vertex not found.");
    }
    for (Edge<Info> *edge: vertexA->getAdj()) {
        if (edge->getDest() == vertexB) {
            return edge;
        }
    }
    return nullptr;
}

// TODO: would it be better to have the pairs of Info as key, instead of pairs of string codes?
//For each examined pipeline, list the affected cities displaying their codes and water supply in deficit.
std::unordered_map<std::pair<std::string, std::string>, std::vector<std::pair<uint16_t, int>>, pair_hash> Data::removingPipes() {
    std::vector<std::pair<uint16_t, uint32_t>> maxFlows = maxFlowCity();

    for (Vertex<Info> *v : g.getVertexSet()) {
        for (Edge<Info> *e: v->getAdj()) {
            e->setSelected(false); // not analyzed yet
        }
    }

    using EdgeKey = std::pair<std::string, std::string>;
    std::unordered_map<EdgeKey, std::vector<std::pair<uint16_t, int>>, pair_hash> pipeImpactMap;

    for (Vertex<Info> *v : g.getVertexSet()) {
        for (Edge<Info> *e : v->getAdj()) {
            if (e->isSelected()) {
                continue;
            }
            // temporarily inactivate edge
            double originalWeight = e->getWeight();
            e->setWeight(0);

            bool isBidirectional = e->getReverse() != nullptr;
            if (isBidirectional) {
                e->getReverse()->setWeight(0);
            }

            std::vector<std::pair<uint16_t, int>> newFlows;
            std::vector<std::pair<uint16_t, uint32_t>> newMaxFlows = maxFlowCity();
            for (const auto &[cityId, originalFlow] : maxFlows) {
                auto it = std::find_if(newMaxFlows.begin(), newMaxFlows.end(),
                                       [cityId](const auto &pair)
                                       { return pair.first == cityId; });
                if (it != newMaxFlows.end()) {
                    newFlows.emplace_back(cityId,  it->second);
                }
            }

            EdgeKey key;
            // par de codes (strings)
            std::string codeA = Utils::parseId(v->getInfo().getKind(), v->getInfo().getId());
            std::string codeB = Utils::parseId(e->getDest()->getInfo().getKind(), e->getDest()->getInfo().getId());

            if (isBidirectional) { // order the pair
                key = (codeA < codeB) ?
                      std::make_pair(codeA, codeB) :
                      std::make_pair(codeB, codeA);
            } else {
                key = std::make_pair(codeA, codeB);
            }

            pipeImpactMap[key] = newFlows;

            e->setWeight(originalWeight);
            e->setSelected(true);

            if (isBidirectional) {
                e->getReverse()->setWeight(originalWeight);
                e->getReverse()->setSelected(true);
            }
        }
    }
    return pipeImpactMap;
}


//For each city, determine which pipelines, if ruptured, i.e., with a null flow capacity,
// would make it impossible to deliver the desired amount of water to a given city.

