#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include <cstdint>
#include <string>
#include <variant>
#include <optional>
#include "../../lib/Graph.h"
#include "../CSV.h"
#include "Info.h"

/**
 * @brief Data storage and algorithms execution.
 * @details This class is responsible for storing the data and executing the algorithms asked by the Runtime class.
 */

class Data {
private:
  /// Graph with the data inside Info objects.
  Graph<Info> g;

  /**
   * @brief Sets the parsed Cities.csv in the graph.
   */
  void setCities(Csv cities);

  /**
   * @brief Sets the parsed Pipes.csv in the graph.
   */
  void setPipes(Csv pipes);

  /**
   * @brief Sets the parsed Reservoir.csv in the graph.
   */
  void setReservoirs(Csv reservoirs);

  /**
   * @brief Sets the parsed Stations.csv in the graph.
   */
  void setStations(Csv stations);

public:
  /**
   * @brief Constructor
   */
  Data(Csv cities, Csv pipes, Csv reservoirs, Csv stations);

  /**
   * @brief Getter for the graph
   */
  Graph<Info> &getGraph();

  /**
   * @brief Number of cities, reservoirs and pumps.
   * @details Useful for debug.
   * @note Time complexity: O(V) where V is the number of vertexes in the graph.
   * @return An array with the number of cities, reservoirs and pumps vertexes, respectively.
   */
  std::array<int, 3> countVertexes();

  /**
   * @brief Maximum amount of water that can reach every city of the graph
   * @details Uses the Edmonds-Karp algorithm to calculate the maximum flow of the graph.
   * @note Time complexity: O(V * E^2) where V is the number of vertexes and E is the number of edges in the graph.
   * @return A map with the city id and the maximum flow that can reach it.
   */
  std::unordered_map<uint16_t, uint32_t> maxFlowCity();

  /**
   * @brief Cities with not enough flow for their demand
   * @details Calculates the maximum flow for every city and selects the ones with flow below the demand.
   * @note Time complexity: O(V * E²) where V is the number of vertexes
   * and E is the number of edges in the graph.
   * @return A vector of pairs with the city Info and the amount of water in deficit.
   * If empty, it means that the network configuration meets the water needs for all of its customer.
   */
  std::vector<std::pair<Info, int32_t>> meetsWaterNeeds();
};


#endif //DA2324_PRJ1_G163_DATA_H
