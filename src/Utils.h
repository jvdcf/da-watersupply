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
private:

public:
  static std::pair<Info::Kind, uint32_t> parseCode(std::string code);
  static std::string parseId(Info::Kind kind, uint32_t id);
  static Vertex<Info>* findVertex(Graph<Info> &g, Info::Kind kind, uint32_t id);

};


#endif // !UTILS
