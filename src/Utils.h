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

  /**
   * @brief Calculate the maximum flow of a graph using the Edmonds-Karp algorithm.
   * @details The return values are inside the graph, the vertexes contain the flow.
   * @note Time complexity: O(V * E^2) where V is the number of vertexes and E is the number of edges in the graph.
   * @param g: A reference to a graph, which vertexes contain Info objects.
   * @param s: A pointer to the source Vertex<Info> object.
   * @param t: A pointer to the target Vertex<Info> object.
   */
  static void EdmondsKarp(Graph<Info> *g, Vertex<Info> *s, Vertex<Info> *t);

  /**
   * @brief Creates an auxiliary Vertex to be used as a super source
   * @details The vertex is added to the graph and connected to all the Reservoirs with infinite capacity.
   * @param g: A pointer to a graph, which vertexes contain Info objects.
   * @return A pointer to the super source vertex.
   */
  static Vertex<Info>* createSuperSource(Graph<Info> *g);

  /**
   * @brief Creates an auxiliary Vertex to be used as a super sink
   * @details The vertex is added to the graph and connected to all the Cities with infinite capacity.
   * @param g: A pointer to a graph, which vertexes contain Info objects.
   * @return A pointer to the super sink vertex.
   */
  static Vertex<Info>* createSuperSink(Graph<Info> *g);

  /**
   * @brief Removes the auxiliary super source vertex from the graph
   * @param g: A pointer to a graph, which vertexes contain Info objects.
   * @param s: A pointer to the super source vertex.
   */
  static void removeSuperSource(Graph<Info> *g, Vertex<Info> *s);

  /**
   * @brief Removes the auxiliary super sink vertex from the graph
   * @param g: A pointer to a graph, which vertexes contain Info objects.
   * @param t: A pointer to the super sink vertex.
   */
  static void removeSuperSink(Graph<Info> *g, Vertex<Info> *t);

};


#endif // !UTILS
