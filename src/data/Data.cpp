#include "Data.h"

#include <utility>

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

