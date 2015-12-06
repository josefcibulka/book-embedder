#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>

#include "tools.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

bool Tools::doEdgesCross(const Edge &e1, const Edge &e2)
{
  if (e1.p != e2.p)
    return false;
  int e1v1 = std::min(e1.v1, e1.v2);
  int e1v2 = std::max(e1.v1, e1.v2);
  int e2v1 = std::min(e2.v1, e2.v2);
  int e2v2 = std::max(e2.v1, e2.v2);
  if (e1v1 < e2v1 && e1v2 > e2v1 && e1v2 < e2v2)
    return true;
  if (e1v1 > e2v1 && e1v1 < e2v2 && e1v2 > e2v2)
    return true;
  return false;
}

/**
 * Counts crossings of ed with the other edges of gr.
 * Time O(m), but faster if ed is short.
 */
int Tools::countEdgeCrossings(const Graph &gr, const Edge &ed)
{
  int result = 0;

  int v1 = std::min(ed.v1, ed.v2);
  int v2 = std::max(ed.v1, ed.v2);

  for (int id2 = v1 + 1; id2 < v2; id2++)
    for (const Edge *ed2 : gr.v[id2].neighs)
    {
      if (ed.p != ed2->p)
        continue;
      int e2V2 = ed2->getOtherEnd(id2);
      assert(doEdgesCross(ed, *ed2) == (e2V2 < v1 || e2V2 > v2));
      if (e2V2 < v1 || e2V2 > v2)
        result++;
    }
  return result;
}

/**
 * Time O(deg(ver)*m) ~ O(m^2/n) (less if the edges are short)
 */
int Tools::countEdgesFromVertexCrossings(const Graph &gr, const Vertex &ver)
{
  int result = 0;
  for (const Edge *ed : ver.neighs)
    result += countEdgeCrossings(gr, *ed);
  return result;
}

/**
 * O(m^2) (one m is smaller if the edges are short)
 */
int Tools::countCrossingNumber(const Graph &gr)
{
  int result = 0;
  for (const Edge &ed : gr.e)
    result += countEdgeCrossings(gr, ed);
  result >>= 1;  // every crossing was counted twice
  return result;
}

/**
 * Counts the change in the number of crossings if we swap v1 and v1+1.
 * Positive return value ... the crossing number increases by the swap.
 * Note that the only crossings that may change are those where one edge has v1 as
 * one of its endpoints and the other edge has v1+1 as one of its end-points.
 * O(deg(v1)*deg(v1+1)) ~ O(m^2/n^2)
 */
int Tools::countCrossingChangeIfNeighborsSwapped(const Graph &gr, int v1)
{
  int result = 0;
  assert(v1 + 1 < static_cast<int>(gr.v.size()));
  const Vertex &ver1 = gr.v[v1];
  const Vertex &ver2 = gr.v[v1 + 1];
  for (const Edge *ed1 : ver1.neighs)
    for (const Edge *ed2 : ver2.neighs)
    {
      if (ed1->p != ed2->p)
        continue;
      int ed1v2 = ed1->getOtherEnd(v1);
      int ed2v2 = ed2->getOtherEnd(v1 + 1);
      if (ed1v2 == ed2v2 || ed1v2 == v1 + 1 || ed2v2 == v1)
        continue;  // they share an endpoint -> they never cross
      // they do not share an endpoint -> they cross either before or after the swap
      bool crossBefore = false;
      if (ed1v2 > v1 + 1 && (ed2v2 < v1 || ed2v2 > ed1v2))
        crossBefore = true;
      else if (ed1v2 < v1 && ed2v2 < v1 && ed2v2 > ed1v2)
        crossBefore = true;
      result += (crossBefore ? -1 : 1);
    }
  return result;
}

/**
 * Counts crossings between pairs of edges, where one edge has v1 as one of its end-points
 * and the other edge has v1+1 as one of its end-points.
 * O(deg(v1)*deg(v1+1)) ~ O(m^2/n^2)
 */
int Tools::countCrossingsOfEdgesFromNeighbors(const Graph &gr, int v1)
{
  int result = 0;
  assert(v1 + 1 < static_cast<int>(gr.v.size()));
  const Vertex &ver1 = gr.v[v1];
  const Vertex &ver2 = gr.v[v1 + 1];
  for (const Edge *ed1 : ver1.neighs)
    for (const Edge *ed2 : ver2.neighs)
    {
      if (ed1->p != ed2->p)
        continue;
      int ed1v2 = ed1->getOtherEnd(v1);
      int ed2v2 = ed2->getOtherEnd(v1 + 1);
      if (ed1v2 > v1 + 1 && (ed2v2 < v1 || ed2v2 > ed1v2))
        result++;
      else if (ed1v2 < v1 && ed2v2 < v1 && ed2v2 > ed1v2)
        result++;
    }
  return result;
}

/**
 * Updates crossing counters of edges from the list; for each crossing of the edge, factor is added to the counter.
 * The result is multiplied by factor.
 * Time O(deg(v1)*|eList|)). That is, when eList are all page variants of edges from a single vertex: roughly O(pageCnt * m^2/n^2)
 */
void Tools::countEdgesVertexCrossingsImpl(const Graph &gr, int v1,
                                          vector<Edge> &eList, int factor)
{
  const Vertex &ver1 = gr.v[v1];
  for (Edge &ed1 : eList)
    for (const Edge *ed2 : ver1.neighs)
    {
      if (ed1.p != ed2->p)
        continue;
      if (doEdgesCross(ed1, *ed2))
        ed1.cr += factor;
    }
}

/**
 * Updates crossing counters of edges from the list; for each crossing of the edge, the counter decreases by 1.
 * Time O(deg(v1)*|eList|)). That is, when eList are all page variants of edges from a single vertex: roughly O(pageCnt * m^2/n^2)
 */
void Tools::removeEdgesVertexCrossings(const Graph &gr, int v1,
                                       vector<Edge> &eList)
{
  countEdgesVertexCrossingsImpl(gr, v1, eList, -1);
}

/**
 * Updates crossing counters of edges from the list; for each crossing of the edge, the counter increases by 1.
 * Time O(deg(v1)*|eList|)). That is, when eList are all page variants of edges from a single vertex: roughly O(pageCnt * m^2/n^2)
 */
void Tools::addEdgesVertexCrossings(const Graph &gr, int v1,
                                    vector<Edge> &eList)
{
  countEdgesVertexCrossingsImpl(gr, v1, eList, 1);
}

/**
 * For every edge in the list, counts the number of crossings with edges of gr.
 * Time O(|eList| * m).
 */
void Tools::countEdgesGraphCrossings(const Graph &gr, vector<Edge> &eList)
{
  for (Edge &ed1 : eList)
    for (const Edge &ed2 : gr.e)
    {
      if (ed1.p != ed2.p)
        continue;
      if (doEdgesCross(ed1, ed2))
        ed1.cr++;
    }
}

/**
 * Move a single vertex from vOld to vNew.
 * Time: O(m)
 */
void Tools::moveVertex(Graph *gr, int vOld, int vNew)
{
  if (vOld == vNew)
    return;
  vector<int> newPos;
  int n = static_cast<int>(gr->v.size());
  for (int i = 0; i < n; i++)
  {
    int nPos = i;
    if (i > vOld)
      nPos--;  // vertices after the old position move back
    if (nPos >= vNew)
      nPos++;  // vertices on or after the new position move forward
    if (i == vOld)
      nPos = vNew;  // the moved vertex
    newPos.push_back(nPos);
  }
  vector<Vertex> newVerVector(n, Vertex(-1));
  for (int i = 0; i < n; i++)
    newVerVector[newPos[i]] = std::move(gr->v[i]);
  gr->v = std::move(newVerVector);

  for (Edge &ed : gr->e)
  {
    ed.v1 = newPos[ed.v1];
    ed.v2 = newPos[ed.v2];
  }

  for (Vertex &ver : gr->v)
    if (ver.id == -1)
    {
      std::cerr << "Vertex " << std::distance(&(gr->v[0]), &ver)
                << " has id -1." << endl;
      exit(0);
    }
}

/**
 * Swap vertices vA and vB.
 * Time: O(deg(vA) + deg(vB))
 */
void Tools::swapVertices(Graph *gr, int vA, int vB)
{
  for (Edge *ed : gr->v[vA].neighs)
  {
    if (ed->v1 == vA && ed->v2 == vB)
      std::swap(ed->v1, ed->v2);
    else if (ed->v1 == vB && ed->v2 == vA)
      std::swap(ed->v1, ed->v2);
    else if (ed->v1 == vA)
      ed->v1 = vB;
    else if (ed->v2 == vA)
      ed->v2 = vB;
  }

  for (Edge *ed : gr->v[vB].neighs)
  {
    // Ignore the edge between vA and vB ... it was already taken care of.
    if (ed->v1 == vB && ed->v2 != vA)
      ed->v1 = vA;
    else if (ed->v2 == vB && ed->v1 != vA)
      ed->v2 = vA;
  }
  std::swap(gr->v[vA], gr->v[vB]);
}

/**
 * Finds the best page for the edge ed.
 * Crossings with edges with unassigned page (their page is negative) are ignored.
 * In case of a tie, a page other than the original is preferred.
 * Time O(m), but faster if ed is short.
 * @return true iff an improvement was done.
 */
bool Tools::greedyEdgePage(Graph *gr, Edge *ed)
{
  int v1 = std::min(ed->v1, ed->v2);
  int v2 = std::max(ed->v1, ed->v2);

  vector<unsigned> pageValue(gr->p, 0);

  for (int id2 = v1 + 1; id2 < v2; id2++)
    for (const Edge *ed2 : gr->v[id2].neighs)
    {
      if (ed2->p < 0)
        continue;
      int e2V2 = ed2->getOtherEnd(id2);
      assert(
          doEdgesCross(*ed, *ed2) == (ed->p == ed2->p && ( e2V2 < v1 || e2V2 > v2)));
      if (e2V2 < v1 || e2V2 > v2)
        pageValue[ed2->p]++;
    }

  int origPage = ed->p;
  int best = origPage;

  // Find the best page other than the original.
  for (int pCur = 0; pCur < gr->p; pCur++)
    if (pCur != origPage)
      if (best < 0 || pageValue[pCur] <= pageValue[best])
        best = pCur;

  if (best < 0)  // may happen if there is only one page
    return false;
  ed->p = best;
  return (origPage < 0 || pageValue[ed->p] < pageValue[origPage]);
}

/**
 * Finds the best page for every edge incident with the vertex at position v.
 * Time O(deg(v) * m), but faster if edges incident to v are short.
 */
void Tools::greedyAtVertex(Graph *gr, int v)
{
  for (Edge *ed : gr->v[v].neighs)
    greedyEdgePage(gr, ed);
}

/**
 * Iterates until the iteration that does not improve.
 * In each iteration, finds the best page for every edge of G.
 * The edges are taken in the order that was given by the input file.
 * Time O(iterCnt * m * m), but faster if edges are short (one of the m's is then smaller).
 */
void Tools::greedyPages(Graph *gr)
{
  bool improved = true;
  while (improved)
  {
    improved = false;
    for (Edge &ed : gr->e)
      if (greedyEdgePage(gr, &ed))
        improved = true;
  }
}

/**
 * The same as greedyPages, but takes the edges in the order of increasing length.
 * Time O(iterCnt * m * m), but faster if edges are short (one of the m's is then smaller).
 */
void Tools::lenPages(Graph *gr)
{
  vector<Edge *> eSorted;
  for (Edge &e : gr->e)
    eSorted.push_back(&e);
  std::sort(eSorted.begin(), eSorted.end(), EdgeLengthComparer());
  bool improved = true;
  while (improved)
  {
    improved = false;
    for (Edge *ed : eSorted)
      if (greedyEdgePage(gr, ed))
        improved = true;
  }
}

/**
 * Set pages of all edges to -1 and then calls the placer function.
 * If the result is worse than the initial, gr is not changed.
 */
int Tools::restartEdges(Graph *gr, int prevCr, void (*placer)(Graph *gr))
{
  Graph grBck = *gr;
  for (Edge &e : gr->e)
    e.p = -1;
  placer(gr);
  int newCr = countCrossingNumber(*gr);
  if (prevCr < newCr)
  {
    gr->loadFrom(grBck);
    return prevCr;
  }
  else
  {
    return newCr;
  }
}

class BestPosFinderEdge
{
 public:
  BestPosFinderEdge(int v2In, int pageCntIn)
      : v2(v2In),
        pageCr(pageCntIn, 0)
  {
  }
  void fillCrossingsAtZero(const Graph &gr)
  {
    for (const Edge &ed : gr.e)
    {
      if (ed.v1 <= v2 && ed.v2 <= v2)  // ed lies under this edge
        continue;
      if (ed.v1 >= v2 && ed.v2 >= v2)  // ed lies to the right of this edge
        continue;
      if (ed.v1 == 0 || ed.v2 == 0)  // ed shares a vertex with this -> no crossing
        continue;
      pageCr[ed.p]++;
    }
  }
  /**
   * Moving from in between v-1 and v to in between v and v+1.
   */
  void updateCrossingsWhenMovingOver(const Graph &gr, int v)
  {
    assert(v != 0);
    for (const Edge *ed : gr.v[v].neighs)
    {
      int edv2 = ed->getOtherEnd(v);
      if (v2 == v || edv2 == 0 || edv2 == v2)  // ed shares a vertex with this -> no crossing
        continue;
      bool crossedBefore = false;
      if (edv2 < v && (v2 > v || v2 < edv2))
        crossedBefore = true;
      if (edv2 > v && v2 > v && v2 < edv2)
        crossedBefore = true;
      pageCr[ed->p] += (crossedBefore ? -1 : 1);
    }
  }

  int v2;
  vector<int> pageCr;
};

/**
 * Find the best position for the vertex, excluding the original position.
 * Returns the change in the crossing number (negative = improvement).
 * The graph gr is unchanged.
 * Time O(deg(origPos)*m) ~ O(m^2/n).
 */
int Tools::findBestPositionForVertex(const Graph &gr, int origPos,
                                     int *finalPos)
{
  assert (gr.v.size() != 1);
  Graph mGr = Graph(gr);
  moveVertex(&mGr, origPos, 0);

  // Number of crossings of edges going from the studied vertex when the vertex is at original position
  // We cannot get this value later, since the vertex might have been not initially greedy-optimal.
  int origDiff = countEdgesFromVertexCrossings(gr, gr.v[origPos]);

  vector<BestPosFinderEdge> allPageE;
  for (Edge *e : mGr.v[0].neighs)
  {
    BestPosFinderEdge newE = BestPosFinderEdge(e->getOtherEnd(0), gr.p);
    newE.fillCrossingsAtZero(mGr);
    allPageE.push_back(newE);
  }

  int bestDiff = 0;  // number of crossings of edges going from the studied vertex when the vertex is at best position (original position excluded)
  int best = -1;
  for (std::size_t j = 0; j < gr.v.size(); j++)
  {
    if (j > 0)
      for (BestPosFinderEdge &e : allPageE)
        e.updateCrossingsWhenMovingOver(mGr, j);

    int curDiff = 0;
    for (const BestPosFinderEdge &ed : allPageE)
      curDiff += *(std::min_element(ed.pageCr.begin(), ed.pageCr.end()));

    if (static_cast<int>(j) == origPos)
      continue;

    if (best < 0 || curDiff <= bestDiff)
    {
      bestDiff = curDiff;
      best = j;
    }
  }
  int retval = bestDiff - origDiff;
  *finalPos = best;
  return retval;
}
