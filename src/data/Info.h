#ifndef DA2324_PRJ1_G163_INFO_H
#define DA2324_PRJ1_G163_INFO_H


#include <cstdint>
#include <string>
#include <variant>
#include <optional>

class Info {
public:
  enum Kind {
    Reservoir,
    Pump,
    City
  };

  struct ReservoirData {
    uint32_t cap;
    std::string location;
    std::string municipality;

    ReservoirData(uint32_t c, std::string l, std::string m): cap(c), location(l), municipality(m) {};
  };

  struct CityData {
    float cap;
    std::string location;
    uint32_t population;

    CityData(float c, std::string l, uint32_t p): cap(c), location(l), population(p) {};
  };

  struct PumpData {
    PumpData() = default;
  };  // Empty, only has ID

  Info(Kind kind, uint16_t id, const std::variant<ReservoirData, PumpData, CityData> &data);

  Kind getKind() const;

  uint16_t getId() const;

  const std::variant<ReservoirData, PumpData, CityData> &getData() const;

  std::optional<float> getCap() const;

  std::optional<std::string> getLocation() const;

  std::optional<uint32_t> getPopulation() const;

  std::optional<std::string> getMunicipality() const;

private:
  Kind kind;
  uint16_t id;
  std::variant<ReservoirData, PumpData, CityData> data;
};


#endif //DA2324_PRJ1_G163_INFO_H
