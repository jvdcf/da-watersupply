#ifndef DA2324_PRJ1_G163_INFO_H
#define DA2324_PRJ1_G163_INFO_H


#include <cstdint>
#include <string>
#include <variant>
#include <optional>

/**
 * @brief Information inside a Vertex.
 * @details This class is used to store information inside a Vertex.\n
 * A vertex can be a Reservoir, a Pump or a City. Depending on the kind of vertex, the data will be different.
 */

class Info {
public:

  /**
   * @brief Enum with the possible kinds of Vertex.
   */
  enum Kind {
    /// Water reservoir, capable of providing water
    Reservoir,
    /// Pumping station, connects reservoirs to cities
    Pump,
    /// Delivery site, connects water supply to the final consumer
    City
  };

  /**
   * @brief Information inside a Reservoir Vertex.
   */
  struct ReservoirData {
    /// Capacity
    uint32_t cap;
    /// Name
    std::string location;
    /// Municipality where it is located
    std::string municipality;
    /// Constructor
    ReservoirData(uint32_t c, std::string l, std::string m): cap(c), location(l), municipality(m) {};
  };

  /**
   * @brief Information inside a City Vertex.
   */
  struct CityData {
    /// Rate demand in m³/s
    float cap;
    /// City of the delivery site
    std::string location;
    /// Total population served
    uint32_t population;
    /// Constructor
    CityData(float c, std::string l, uint32_t p): cap(c), location(l), population(p) {};
  };

  /**
   * @brief Information inside a Pump Vertex.
   * @details This struct is empty.
   */
  struct PumpData {
    /// Constructor
    PumpData() = default;
  };

  /**
   * @brief Constructor
   * @param kind: Kind of Vertex
   * @param id: Id number
   * @param data: Data for the Vertex. This variant can hold a ReservoirData, a PumpData or a CityData.
   */
  Info(Kind kind, uint16_t id, const std::variant<ReservoirData, PumpData, CityData> &data);

  /**
   * @brief Getter for the kind of Vertex
   * @return Info::Kind
   */
  Kind getKind() const;

  /**
   * @brief Getter for the id number
   * @return Id number
   */
  uint16_t getId() const;

  /**
   * @brief Getter for the data inside
   * @warning It is more appropriate to use the other getters:\n
   * - Info::getCap()\n
   * - Info::getLocation()\n
   * - Info::getPopulation()\n
   * - Info::getMunicipality()
   * @return A variant with the data
   */
  const std::variant<ReservoirData, PumpData, CityData> &getData() const;

  /**
   * @brief Getter for the capacity / demand of the Vertex
   * @details Only works if the kind of Vertex is a Reservoir or a City.
   * @return Capacity in m³/s
   */
  std::optional<float> getCap() const;

  /**
   * @brief Getter for the name / city of the Vertex
   * @details Only works if the kind of Vertex is a Reservoir or a City.
   * @return String with the name
   */
  std::optional<std::string> getLocation() const;

  /**
   * @brief Getter for the population served by the city
   * @details Only works if the kind of Vertex is a City.
   * @return Integer with the population served
   */
  std::optional<uint32_t> getPopulation() const;

  /**
   * @brief Getter for the municipality of the reservoir
   * @details Only works if the kind of Vertex is a Reservoir.
   * @return String with the municipality
   */
  std::optional<std::string> getMunicipality() const;

  /**
   * @brief Equality operator
   */
  bool operator==(const Info &rhs) const;

private:
  /// Kind of Vertex (Reservoir, Pump or City)
  Kind kind;
  /// Id number
  uint16_t id;
  /// Data for the Vertex (ReservoirData, PumpData or CityData)
  std::variant<ReservoirData, PumpData, CityData> data;
};


#endif //DA2324_PRJ1_G163_INFO_H
