#include "Utils.h"

std::pair<Info::Kind, uint32_t> Utils::parseCode(std::string code) {
  char charKind = code[0];
  switch (charKind) {
    case 'C':
      if (code[1] != '_') std::cerr << "ERROR: " << code << "is an invalid code.\n";
      return {Info::Kind::City, std::stoi(code.substr(2))};
    case 'R':
      if (code[1] != '_') std::cerr << "ERROR: " << code << "is an invalid code.\n";
      return {Info::Kind::Reservoir, std::stoi(code.substr(2))};
    case 'P':
      if (code[1] != 'S' | code[2] != '_') std::cerr << "ERROR: " << code << "is an invalid code.\n";
      return {Info::Kind::Pump, std::stoi(code.substr(3))};
    default:
      std::cerr << "ERROR: " << code << "is an invalid code.\n";
  }
}

std::string Utils::parseId(Info::Kind kind, uint32_t id) {
  std::string head;
  switch (kind) {
    case Info::Kind::City:
      head = "C_";
      break;
    case Info::Kind::Reservoir:
      head = "R_";
      break;
    case Info::Kind::Pump:
      head = "PS_";
      break;
  }
  return head + std::to_string(id);
}

Vertex<Info> *Utils::findVertex(Graph<Info> &g, Info::Kind kind, uint32_t id) {
  // TODO: Optimize search using the already ordered vector
  for (Vertex<Info>* v : g.getVertexSet()) {
    if (v->getInfo().getKind() == kind && v->getInfo().getId() == id) {
      return v;
    }
  }
  std::cerr << "ERROR: Could not find vertex for " << parseId(kind, id) << std::endl;
  return nullptr;
}
