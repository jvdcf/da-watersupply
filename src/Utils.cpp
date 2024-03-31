#include <functional>
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

void Utils::EdmondsKarp(Graph<Info> *g, Vertex<Info> *s, Vertex<Info> *t) {
  auto testAndVisit = [](std::queue< Vertex<Info>*> &q, Edge<Info> *e, Vertex<Info> *w, double residual) {
    if (! w->isVisited() && residual > 0 && w->isActive()) {
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

  for (auto v : g->getVertexSet()) {
    for (auto e: v->getAdj()) {
      e->setFlow(0);
    }
  }

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
      g->addEdge(s, v, INF);
    }
  }
  return s;
}

Vertex<Info> *Utils::createSuperSink(Graph<Info> *g) {
  g->addVertex(Info(Info::Kind::Pump, INT16_MAX, Info::PumpData()));
  Vertex<Info> *t = g->getVertexSet().back();
  for (auto v: g->getVertexSet()) {
    if (v->getInfo().getKind() == Info::Kind::City) {
      g->addEdge(v, t, INF);
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
