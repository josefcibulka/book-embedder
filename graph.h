/**
 * The class for storing graphs in book-embedder.
 *
 * Program for minimizing the number of crossings in a book embedding of a given
 * graph to a given number of pages.
 *
 * Author: Josef Cibulka
 * License: see the file LICENSE
 */
#ifndef BOOK_EMBEDDER_GRAPH_H_
#define BOOK_EMBEDDER_GRAPH_H_

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>

class Edge
{
 public:
  int v1, v2;  ///< The current positions of the endpoints (may differ from the vertex ids)
  int p = 0;  ///< The page, where the edge currently is (numbering starts at 0)
  int cr = 0;  ///< Number of crossings. Used only sometimes.
  Edge(int v1_in, int v2_in, int p_in)
      : v1(v1_in),
        v2(v2_in),
        p(p_in)
  {
  }
  int getOtherEnd(int va) const
  {
    assert(v1 == va || v2 == va);
    if (v1 == va)
      return v2;
    else
      return v1;
  }
};

/**
 * Function object for sorting edges by increasing length.
 */
class EdgeLengthComparer
{
 public:
  inline bool operator()(const Edge* e1, const Edge* e2)
  {
    return (std::abs(e1->v1 - e1->v2) > std::abs(e2->v1 - e2->v2));
  }
};

/**
 * The vertex with an id, which is the initial position.
 * The current position is the position within the vector in Graph.
 */
class Vertex
{
 public:
  int id;  ///< The initial position.
  std::vector<Edge *> neighs;
  Vertex(int id_in)
      : id(id_in)
  {
  }
};

class Graph
{
 public:
  int p = 0;
  std::vector<Vertex> v;  ///< The vertices ordered as in the drawing.
  std::vector<Edge> e;  ///< If changed (or resized), neighbor lists in vertices must be updated!!!

  Graph()
  {
  }

  Graph(const Graph& orig)
      : p(orig.p),
        v(orig.v),
        e(orig.e)
  {
    restoreNeighs();
  }

  Graph& operator=(const Graph& other) = delete;

  void loadFrom(const Graph &other)
  {
    p = other.p;
    v = other.v;
    e = other.e;
    restoreNeighs();
  }

  /**
   * Fills the neighs in all the vertices based on the information in edges.
   */
  void restoreNeighs()
  {
    for (Vertex &ver : v)
      ver.neighs.clear();
    for (Edge &ed : e)
    {
      v[ed.v1].neighs.push_back(&ed);
      v[ed.v2].neighs.push_back(&ed);
    }
  }
};

#endif
