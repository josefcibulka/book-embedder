#ifndef BOOK_EMBEDDER_TOOLS_H_
#define BOOK_EMBEDDER_TOOLS_H_

#include <vector>

#include "graph.h"

class Tools
{
 public:
  static bool doEdgesCross(const Edge &e1, const Edge &e2);

  static int countEdgeCrossings(const Graph &gr, const Edge &ed);

  static int countEdgesFromVertexCrossings(const Graph &gr, const Vertex &ver);

  static int countCrossingNumber(const Graph &gr);

  static int countEdgesFromNeighborsCrossings(const Graph &gr, int v1);

  static int removeEdgesVertexCrossings(const Graph &gr, int v1, std::vector<Edge> &eList);

  static int addEdgesVertexCrossings(const Graph &gr, int v1, std::vector<Edge> &eList);

  static int addEdgesGraphCrossings(const Graph &gr, std::vector<Edge> &eList);

  static void moveVertex(Graph *gr, int vOld, int vNew);

  static void swapVertices(Graph *gr, int vA, int vB);

  static bool greedyEdgePage(Graph *gr, Edge *ed);

  static void greedyAtVertex(Graph *gr, int v);

  static void greedyPages(Graph *gr);

  static int findBestPositionForVertex(const Graph &gr, int origPos, int *finalPos);

  static void BaurBrandes(Graph *gr);

  static void lenPages(Graph *gr);

  static int restartEdges(Graph *gr, int prevCr, void (*placer)(Graph *gr));

 private:
  static int countEdgesVertexCrossingsImpl(const Graph &gr, int v1, std::vector<Edge> &eList,
                                    int factor);
};


#endif /* BOOK_EMBEDDER_TOOLS_H_ */
