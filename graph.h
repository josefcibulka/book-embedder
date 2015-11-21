#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>

class Edge
{
public:
   int v1, v2;
   int p = 0;
   int cr = 0; ///< number of crossings. Used only sometimes.
   Edge(int v1_in, int v2_in)
         : v1(v1_in), v2(v2_in)
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
 * For sorting edges by increasing length.
 */
class EdgeLengthComparer
{
public:
   inline bool operator()(const Edge* e1, const Edge* e2)
   {
      return (std::abs(e1->v1 - e1->v2) > std::abs(e2->v1 - e2->v2));
   }
};

class Vertex
{
public:
   int id;
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
   std::vector<Vertex> v;
   std::vector<Edge> e;  ///< If changed (or resized), neighbor lists in vertices must be updated!!!

   Graph()
   {
   }

   Graph(const Graph& orig)
         : p(orig.p), v(orig.v), e(orig.e)
   {
      updateNeighs();
   }
   Graph& operator=(const Graph& other) = delete;

   void loadFrom(const Graph &other)
   {
      p = other.p;
      v = other.v;
      e = other.e;
      updateNeighs();
   }

   void updateNeighs()
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
