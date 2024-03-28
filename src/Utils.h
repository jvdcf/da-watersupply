#ifndef UTILS
#define UTILS


#include <utility>
#include "data/Info.h"
#include "../lib/Graph.h"

/**
 * @brief Auxiliary functions
 * @details This class contains static and auxiliary functions used throughout the project.
 */

class Utils {
public:
  /**
   * @brief Parses a code into an id and an Info::Kind of Vertex
   * @param code: A string with the format "x_y" where x is the kind of Vertex ('C', 'R' or 'PS') and y is the id number.
   * @return Info::Kind (City, Reservoir or Station) and id.
   */
  static std::pair<Info::Kind, uint32_t> parseCode(std::string code);

  /**
   * @brief Parses an id and an Info::Kind into a code
   * @param kind: Info::Kind (City, Reservoir or Station)
   * @param id: Id number
   * @return A string with the format "x_y" where x is the kind of Vertex ('C', 'R' or 'PS') and y is the id number.
   */
  static std::string parseId(Info::Kind kind, uint32_t id);

  /**
   * @brief Finds a Vertex in a given graph.
   * @note Time complexity: O(V) where V is the number of vertexes in the graph.
   * @param g: A reference to a graph, which vertexes contain Info objects.
   * @param kind: Info::Kind (City, Reservoir or Station)
   * @param id: Id number
   * @return A pointer to the Vertex<Info> object if found, nullptr otherwise.
   */
  static Vertex<Info>* findVertex(Graph<Info> &g, Info::Kind kind, uint32_t id);

};


#endif // !UTILS
