#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include <cstdint>
#include <string>
#include <variant>
#include <optional>
#include "../../lib/Graph.h"
#include "../CSV.h"
#include "Info.h"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

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
   * @brief Number of cities, reservoirs and pumps.
   * @details Useful for debug.
   * @note Time complexity: O(V) where V is the number of vertexes in the graph.
   * @return An array with the number of cities, reservoirs and pumps vertexes, respectively.
   */
  std::array<int, 3> countVertexes();

  /**
   * @brief Maximum amount of water that can reach each or a specific city
   * @details Uses the Edmonds-Karp algorithm to calculate the maximum flow of the graph.
   * @note Time complexity: O(V * E^2) where V is the number of vertexes and E is the number of edges in the graph.
   * @return A vector of pairs with the city id and the maximum flow that can reach it.
   */
  std::vector<std::pair<uint16_t, uint32_t>> maxFlowCity();

    /**
     * @brief Impact in each city of removing each pump station
     * @details Calculates the flow arriving at each city after inactivating each pump station.
     * @note Time complexity: O(V^2 * E^2) where V is the number of vertexes and E is the number of edges in the graph.
     * @return A map with the Info of the removed pump stations and a vector of pairs with the city id and the resulting deficit.
     */
     std::unordered_map<Info, std::vector<std::pair<uint16_t, int>>> removingPumps();

    /**
     * @brief Finds an Edge between two vertices the graph.
     * @note Time complexity: O(V + E) where V is the number of vertices and E is the number of edges in the graph.
     * @param srcId: Id of the source vertex.
     * @param destId: Id of the destination vertex.
     * @return A pointer to the Edge<Info> object if found, nullptr otherwise.
     */
    Edge<Info> *findEdge(uint32_t srcId, uint32_t destId);

    /**
     * @brief Impact in each city of removing each pipe
     * @details Calculates the flow arriving at each city after removing each pipe.
     * @note Time complexity: O((V+E) * V * E^2) where V is the number of vertexes and E is the number of edges in the graph.
     * @return A map with the pair of the source and destination vertexes of the removed pipes and a vector of pairs with the city id and the resulting deficit.
     */
    std::unordered_map<std::pair<uint16_t, uint16_t>, std::vector<std::pair<uint16_t, int>>, pair_hash> removingPipes();

};




#endif //DA2324_PRJ1_G163_DATA_H
