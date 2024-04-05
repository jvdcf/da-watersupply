#include "Data.h"
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

// Constructor

Data::Data(Csv cities, Csv pipes, Csv reservoirs, Csv stations) {
  this->g = Graph<Info>();
  setCities(std::move(cities));
  setReservoirs(std::move(reservoirs));
  setStations(std::move(stations));
  setPipes(std::move(pipes));
}

void Data::setCities(Csv cities) {
  std::vector<CsvLine> data = cities.to_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      warning("Empty line in Cities.csv");
      continue;
    }
    if (!values[0].get_str().has_value())
      panic("Incorrect type: Expected string, but found " +
            values[0].display());
    std::string location = values[0].get_str().value();
    if (!values[1].get_int().has_value())
      panic("Incorrect type: Expected int, but found " + values[1].display());
    uint32_t id = values[1].get_int().value();
    if (!values[3].get_flt().has_value())
      panic("Incorrect type: Expected float, but found " + values[3].display());
    float demand = values[3].get_flt().value();
    if (!values[4].get_int().has_value())
      panic("Incorrect type: Expected int, but found " + values[4].display());
    uint32_t population = values[4].get_int().value();

    const Info info = Info(Info::Kind::City, id,
                           Info::CityData(demand, location, population));
    g.addVertex(info);
  }
}

void Data::setReservoirs(Csv reservoirs) {
  std::vector<CsvLine> data = reservoirs.to_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      warning("Empty line in Reservoir.csv");
      continue;
    }

    if (!values[0].get_str().has_value())
      panic("Incorrect type: Expected string, but found " +
            values[0].display());
    std::string reservoir = values[0].get_str().value();
    if (!values[1].get_str().has_value())
      panic("Incorrect type: Expected string, but found " +
            values[1].display());
    std::string municipality = values[1].get_str().value();
    if (!values[2].get_int().has_value())
      panic("Incorrect type: Expected int, but found " + values[2].display());
    uint32_t id = values[2].get_int().value();
    if (!values[4].get_int().has_value())
      panic("Incorrect type: Expected int, but found " + values[4].display());
    uint32_t capacity = values[4].get_int().value();

    const Info info =
        Info(Info::Kind::Reservoir, id,
             Info::ReservoirData(capacity, municipality, reservoir));
    g.addVertex(info);
  }
}

void Data::setStations(Csv stations) {
  std::vector<CsvLine> data = stations.to_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      warning("Empty line in Stations.csv");
      continue;
    }

    if (!values[0].get_int().has_value())
      panic("Incorrect type: Expected int, but found" + values[0].display());
    uint32_t id = values[0].get_int().value();

    const Info info = Info(Info::Kind::Pump, id, Info::PumpData());
    g.addVertex(info);
  }
}

void Data::setPipes(Csv pipes) {
  std::vector<CsvLine> data = pipes.to_data();
  for (CsvLine line : data) {
    std::vector<CsvValues> values = line.get_data();
    if (values.empty()) {
      warning("Empty line in Pipes.csv");
      continue;
    }

    if (!values[0].get_str().has_value())
      panic("Incorrect type: Expected string, but found" + values[0].display());
    std::string serviceA = values[0].get_str().value();
    if (!values[1].get_str().has_value())
      panic("Incorrect type: Expected string, but found" + values[1].display());
    std::string serviceB = values[1].get_str().value();
    if (!values[2].get_int().has_value())
      panic("Incorrect type: Expected int, but found" + values[2].display());
    uint32_t capacity = values[2].get_int().value();
    if (!values[3].get_int().has_value())
      panic("Incorrect type: Expected int, but found" + values[3].display());
    bool unidirectional = values[3].get_int().value();

    std::pair<Info::Kind, uint32_t> parsedA = Utils::parseCode(serviceA);
    std::pair<Info::Kind, uint32_t> parsedB = Utils::parseCode(serviceB);
    Vertex<Info> *vertexA = Utils::findVertex(g, parsedA.first, parsedA.second);
    Vertex<Info> *vertexB = Utils::findVertex(g, parsedB.first, parsedB.second);

    if (vertexA == nullptr || vertexB == nullptr)
      continue;

    if (!unidirectional)
      g.addBidirectionalEdge(vertexA, vertexB, capacity);
    else
      g.addEdge(vertexA, vertexB, capacity);
  }
}

Graph<Info> &Data::getGraph() { return g; }

// Functions
// =================================================================================================

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

std::unordered_map<uint16_t, uint32_t> Data::maxFlowCity() {
  Vertex<Info> *superSource = Utils::createSuperSource(&g);
  Vertex<Info> *superSink = Utils::createSuperSink(&g);

  Utils::EdmondsKarp(&g, superSource, superSink);

  std::unordered_map<uint16_t, uint32_t> result;
  for (Vertex<Info> *v : g.getVertexSet()) {
    if (v->getInfo().getKind() != Info::Kind::City)
      continue;
    if (v->getInfo().isActive()) {
      result.insert(
          {v->getInfo().getId(), Utils::calcFlow(&this->getGraph(), v)});
    }
  }

  Utils::removeSuperSource(&g, superSource);
  Utils::removeSuperSink(&g, superSink);
  return result;
}

std::vector<std::tuple<uint16_t, uint32_t, uint32_t>>
Data::removeReservoir(uint16_t id) {
  auto all_before = maxFlowCity();
  Vertex<Info> *to_be_deactivated;
  for (auto vx : this->getGraph().getVertexSet()) {
    if (vx->getInfo().getKind() == Info::Kind::Reservoir &&
        vx->getInfo().getId() == id) {
      to_be_deactivated = vx;
    }
  }
  if (to_be_deactivated == nullptr)
    panic("removeReservoir is broken?");
  Info inf = to_be_deactivated->getInfo();
  inf.disable();
  to_be_deactivated->setInfo(inf);
  auto all_after = maxFlowCity();
  inf.enable();
  to_be_deactivated->setInfo(inf);
  std::vector<std::tuple<uint16_t, uint32_t, uint32_t>> res;
  for (auto [bid, flow] : all_before) {
    uint32_t new_flow = all_after.at(bid);
    if (new_flow < flow) {
      res.push_back(std::tuple(bid, flow, new_flow));
    }
  }
  return res;
}

std::vector<std::tuple<uint16_t, uint32_t, uint32_t>>
Data::removePump(uint16_t id) {
  auto all_before = maxFlowCity();
  Vertex<Info> *to_be_deactivated;
  for (auto vx : this->getGraph().getVertexSet()) {
    if (vx->getInfo().getKind() == Info::Kind::Pump &&
        vx->getInfo().getId() == id) {
      to_be_deactivated = vx;
    }
  }
  if (to_be_deactivated == nullptr)
    panic("removePump is broken?");
  Info inf = to_be_deactivated->getInfo();
  inf.disable();
  to_be_deactivated->setInfo(inf);
  auto all_after = maxFlowCity();
  inf.enable();
  to_be_deactivated->setInfo(inf);
  std::vector<std::tuple<uint16_t, uint32_t, uint32_t>> res;
  for (auto [bid, flow] : all_before) {
    uint32_t new_flow = all_after.at(bid);
    if (new_flow < flow) {
      res.push_back(std::tuple(bid, flow, new_flow));
    }
  }
  return res;
}

std::vector<std::pair<Info, int32_t>> Data::meetsWaterNeeds() {
  std::vector<std::pair<Info, int32_t>> result;
  Vertex<Info> *superSource = Utils::createSuperSource(&g);
  Vertex<Info> *superSink = Utils::createSuperSink(&g);

  Utils::EdmondsKarp(&g, superSource, superSink);
  for (Vertex<Info> *v : g.getVertexSet()) {
    if (v->getInfo().getKind() != Info::Kind::City)
      continue;
    double flow = 0;
    for (Edge<Info> *e : v->getIncoming())
      flow += e->getFlow();
    int32_t deficit = round(v->getInfo().getCap().value() - flow);
    if (deficit > 0) {
      result.emplace_back(v->getInfo(), deficit);
    }
  }

  Utils::removeSuperSource(&g, superSource);
  Utils::removeSuperSink(&g, superSink);
  return result;
}

std::unordered_map<Info, std::unordered_map<uint16_t, uint32_t>>
Data::removingPumps() {
  std::unordered_map<Info, std::unordered_map<uint16_t, uint32_t>>
      pumpImpactMap;
  for (Vertex<Info> *v : g.getVertexSet()) {
    if (v->getInfo().getKind() == Info::Kind::Pump) {
      Info fu = v->getInfo();
      fu.disable();
      std::unordered_map<uint16_t, uint32_t> newFlows = maxFlowCity();
      pumpImpactMap[v->getInfo()] = newFlows;
      fu.enable();
      v->setInfo(fu);
    }
  }
  return pumpImpactMap;
}

std::vector<std::tuple<uint16_t, uint32_t, uint32_t>>
  Data::removePipe(std::string id1, std::string id2) {
  auto [k1, c1] = Utils::parseCode(id1);
  auto [k2, c2] = Utils::parseCode(id2);
  Vertex<Info>* v1 = Utils::findVertex(this->getGraph(), k1, c1);
  Vertex<Info>* v2 = Utils::findVertex(this->getGraph(), k2, c2);
  if (v1 == nullptr || v2 == nullptr) {
    error("No such vertices " + id1 + " " + id2 + ".");
    return {};
  }
  Edge<Info>* to_1_2 = Utils::findEdge(v1, v2);
  Edge<Info>* to_2_1 = Utils::findEdge(v2, v1);
  if (nullptr == to_2_1 && to_1_2 == nullptr) {
    error("There are no edges between " + id1 + " and " + id2 + ".");
    return {}; 
  }
  if (to_1_2 != nullptr) to_1_2->setSelected(true);
  if (to_2_1 != nullptr) to_1_2->setSelected(true);

}




// For each examined pipeline, list the affected cities displaying their codes
// and water supply in deficit.
std::unordered_map<std::pair<std::string, std::string>,
                   std::unordered_map<uint16_t, uint32_t>, pair_hash>
Data::removingPipes() {
  std::unordered_map<uint16_t, uint32_t> maxFlows = maxFlowCity();

  for (Vertex<Info> *v : g.getVertexSet()) {
    for (Edge<Info> *e : v->getAdj()) {
      e->setSelected(false); // not analyzed yet
    }
  }

  using EdgeKey = std::pair<std::string, std::string>;
  std::unordered_map<EdgeKey, std::unordered_map<uint16_t, uint32_t>, pair_hash>
      pipeImpactMap;

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

      std::unordered_map<uint16_t, uint32_t> newMaxFlows = maxFlowCity();

      EdgeKey key;
      std::string codeA =
          Utils::parseId(v->getInfo().getKind(), v->getInfo().getId());
      std::string codeB = Utils::parseId(e->getDest()->getInfo().getKind(),
                                         e->getDest()->getInfo().getId());

      if (isBidirectional) { // order the pair
        key = (codeA < codeB) ? std::make_pair(codeA, codeB)
                              : std::make_pair(codeB, codeA);
      } else {
        key = std::make_pair(codeA, codeB);
      }

      pipeImpactMap[key] = newMaxFlows;

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

// For each city, determine which pipelines, if ruptured, i.e., with a null flow
// capacity,
//  would make it impossible to deliver the desired amount of water to a given
//  city.
