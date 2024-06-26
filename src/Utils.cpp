#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include "Utils.h"



[[noreturn]] void panic(std::string s) {
  auto c = Color(255,100,100);
  std::cerr << c.foreground() << "[CRITICAL ERR] " << c.clear() << s << std::endl;
  std::exit(1);
}

void error(std::string s) {
  auto c = Color(255,100,0);
  std::cerr << c.foreground() << "[ERROR] " << c.clear() << s << std::endl;
}

void info(std::string s) {
  auto c = Color(0,235,235);
  std::cerr << c.foreground() << "[INFO] " << c.clear() << s << std::endl;
}

void warning(std::string s) {
  auto c = Color(255,255,15);
  std::cerr << c.foreground() <<"[WARNING] " << c.clear() << s << std::endl;
}

Edge<Info>* Utils::findEdge(Vertex<Info>* vertexA, Vertex<Info>* vertexB) {
    if (!vertexA || !vertexB) {
        throw std::runtime_error("Source or destination vertex not found.");
    }
    for (Edge<Info> *edge: vertexA->getAdj()) {
        if (edge->getDest() == vertexB) {
            return edge;
        }
    }
    return nullptr;
}
std::pair<Info::Kind, uint32_t> Utils::parseCode(std::string code) {
  char charKind = code[0];
  switch (charKind) {
    case 'C':
      if (code[1] != '_') {
        error(std::string(code.c_str()) + " is an invalid code");
        throw std::exception();
      }
      return {Info::Kind::City, std::stoi(code.substr(2))};
    case 'R':
      if (code[1] != '_') {
        error(std::string(code.c_str()) + " is an invalid code");
        throw std::exception();
      }
      return {Info::Kind::Reservoir, std::stoi(code.substr(2))};
    case 'P':
      if (code[1] != 'S' || code[2] != '_') {
        error(std::string(code.c_str()) + " is an invalid code");
        throw std::exception();
      }
      return {Info::Kind::Pump, std::stoi(code.substr(3))};
    default:
      error(std::string(code.c_str()) + " is an invalid code");
      throw std::exception();
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
  error("Could not find vertex for " + parseId(kind, id));
  return nullptr;
}

void Utils::EdmondsKarp(Graph<Info> *g, Vertex<Info> *s, Vertex<Info> *t) {
  auto testAndVisit = [](std::queue< Vertex<Info>*> &q, Edge<Info> *e, Vertex<Info> *w, double residual) {
    if (!w->getInfo().isActive()) return;
    if (! w->isVisited() && residual > 0) {
      w->setVisited(true);
      w->setPath(e);
      q.push(w);
    }
  };

  auto findAugmentingPath = [&testAndVisit](Graph<Info> *g, Vertex<Info> *s, Vertex<Info> *t) {
    for (auto v: g->getVertexSet()) v->setVisited(false);
    s->setVisited(true);
    std::queue<Vertex<Info> *> q;
    q.push(s);

    while (!q.empty() && !t->isVisited()) {
      auto v = q.front();
      q.pop();
      for (auto e: v->getAdj()) testAndVisit(q, e, e->getDest(), e->getWeight() - e->getFlow());
      for (auto e: v->getIncoming()) testAndVisit(q, e, e->getOrig(), e->getFlow());
    }
    return t->isVisited();
  };

  auto findMinResidualAlongPath = [](Vertex<Info> *s, Vertex<Info> *t) {
    double f = INF;
    for (auto v = t; v != s; ) {
      auto e = v->getPath();
      if (e->getDest() == v) {
        f = std::min(f, e->getWeight() - e->getFlow());
        v = e->getOrig();
      }
      else {
        f = std::min(f, e->getFlow());
        v = e->getDest();
      }
    }
    return f;
  };

  auto augmentFlowAlongPath = [](Vertex<Info> *s, Vertex<Info> *t, double f) {
    for (auto v = t; v != s; ) {
      auto e = v->getPath();
      double flow = e->getFlow();
      if (e->getDest() == v) {
        e->setFlow(flow + f);
        v = e->getOrig();
      }
      else {
        e->setFlow(flow - f);
        v = e->getDest();
      }
    }
  };

  if (s == nullptr || t == nullptr || s == t)
    throw std::logic_error("Invalid source and/or target vertex");

  while (findAugmentingPath(g, s, t)) {
    double f = findMinResidualAlongPath(s, t);
    augmentFlowAlongPath(s, t, f);
  }
}

Vertex<Info> *Utils::createSuperSource(Graph<Info> *g) {
  g->addVertex(Info(Info::Kind::Pump, INT16_MAX - 1, Info::PumpData()));
  Vertex<Info> *s = g->getVertexSet().back();
  for (auto v: g->getVertexSet()) {
    if (v->getInfo().getKind() == Info::Kind::Reservoir) {
      g->addEdge(s, v, v->getInfo().getCap().value());
    }
  }
  return s;
}

Vertex<Info> *Utils::createSuperSink(Graph<Info> *g) {
  g->addVertex(Info(Info::Kind::Pump, INT16_MAX, Info::PumpData()));
  Vertex<Info> *t = g->getVertexSet().back();
  for (auto v: g->getVertexSet()) {
    if (v->getInfo().getKind() == Info::Kind::City) {
      g->addEdge(v, t, v->getInfo().getCap().value());
    }
  }
  return t;
}

void Utils::removeSuperSource(Graph<Info> *g, Vertex<Info> *s) {
  for (auto e: s->getAdj()) {
    g->removeEdge(s->getInfo(), e->getDest()->getInfo());
  }
  g->removeVertex(s->getInfo());
}

void Utils::removeSuperSink(Graph<Info> *g, Vertex<Info> *t) {
  for (auto e: t->getIncoming()) {
    g->removeEdge(e->getOrig()->getInfo(), t->getInfo());
  }
  g->removeVertex(t->getInfo());
}


uint32_t Utils::calcFlow(Graph<Info> *g, Vertex<Info> *t) {
  uint32_t flow = 0;   
  for (Edge<Info> *e: t->getIncoming()) flow += round(e->getFlow());
  return flow;
}
