#include "Info.h"

Info::Kind Info::getKind() const {
  return kind;
}

uint16_t Info::getId() const {
  return id;
}

const std::variant<Info::ReservoirData, Info::PumpData, Info::CityData> &Info::getData() const {
  return data;
}

Info::Info(Info::Kind kind, uint16_t id, const std::variant<ReservoirData, PumpData, CityData> &data) : kind(kind), id(id), data(data) {}

std::optional<float> Info::getCap() const {
  switch (getKind()) {
    case Pump:
      return {};
    case Reservoir:
      return std::get<ReservoirData>(getData()).cap;
    case City:
      return std::get<CityData>(getData()).cap;
  }
}

std::optional<std::string> Info::getLocation() const {
  switch (getKind()) {
    case Pump:
      return {};
    case Reservoir:
      return std::get<ReservoirData>(getData()).location;
    case City:
      return std::get<CityData>(getData()).location;
  }
}

std::optional<uint32_t> Info::getPopulation() const {
  switch (getKind()) {
    case City:
      return std::get<CityData>(getData()).population;
    default:
      return {};
  }
}

std::optional<std::string> Info::getMunicipality() const {
  switch (getKind()) {
    case Reservoir:
      return std::get<ReservoirData>(getData()).municipality;
    default:
      return {};
  }
}

bool Info::operator==(const Info &rhs) const {
  return kind == rhs.kind && id == rhs.id;
}
