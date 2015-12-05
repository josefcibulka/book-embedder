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
 * O(m) (less if the edge is short)
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
 * O(deg(ver)*m) ~ O(m^2/n) (less if the edges are short)
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
  result >>= 1;
  return result;
}

/**
 * Counts crossings between pairs of edges, where one edge has v1 as one of its end-points
 * and the other edge has v1+1 as one of its end-points.
 * O(deg(v1)*deg(v1+1)) ~ O(m^2/n^2)
 */
int Tools::countEdgesFromNeighborsCrossings(const Graph &gr, int v1)
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
 * Counts crossings between pairs of edges, where one id from eList and the other starts at v1.
 * O(deg(v1)*|eList|)) ~ O(m^2/n^2)
 */
int Tools::countEdgesVertexCrossingsImpl(const Graph &gr, int v1,
                                         vector<Edge> &eList, int factor)
{
  int result = 0;
  const Vertex &ver1 = gr.v[v1];
  for (Edge &ed1 : eList)
    for (const Edge *ed2 : ver1.neighs)
    {
      if (ed1.p != ed2->p)
        continue;
      if (doEdgesCross(ed1, *ed2))
      {
        result += factor;
        ed1.cr += factor;
      }
    }
  return result;
}

int Tools::removeEdgesVertexCrossings(const Graph &gr, int v1,
                                      vector<Edge> &eList)
{
  return countEdgesVertexCrossingsImpl(gr, v1, eList, -1);
}

int Tools::addEdgesVertexCrossings(const Graph &gr, int v1, vector<Edge> &eList)
{
  return countEdgesVertexCrossingsImpl(gr, v1, eList, 1);
}

int Tools::addEdgesGraphCrossings(const Graph &gr, vector<Edge> &eList)
{
  int result = 0;
  for (Edge &ed1 : eList)
    for (const Edge &ed2 : gr.e)
    {
      if (ed1.p != ed2.p)
        continue;
      if (doEdgesCross(ed1, ed2))
      {
        result++;
        ed1.cr++;
      }
    }
  return result;
}

/**
 * Move a single vertex from vOld to vNew.
 */
void Tools::moveVertex(Graph *gr, int vOld, int vNew)
{
  //cerr << "moving " << vOld << " -> " << vNew << endl;
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

bool Tools::greedyEdgePage(Graph *gr, Edge *ed)
{
  bool improved = false;
  int origCr = countEdgeCrossings(*gr, *ed);
  int origPage = ed->p;
  int bestCr = origCr;
  int best = origPage;
  for (int pNew = 0; pNew < gr->p; pNew++)
    if (pNew != origPage)
    {
      ed->p = pNew;
      int curCr = countEdgeCrossings(*gr, *ed);
      if (curCr <= bestCr || best < 0)
      {
        if (curCr < bestCr || best < 0)
          improved = true;
        bestCr = curCr;
        best = pNew;
      }
    }
  ed->p = best;
  return improved;
}

void Tools::greedyAtVertex(Graph *gr, int v)
{
  for (Edge *ed : gr->v[v].neighs)
    greedyEdgePage(gr, ed);
}

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
 * Find the best position for the vertex, excluding the original position.
 * Returns the change in the crossing number (negative = improvement).
 */
int Tools::findBestPositionForVertex(const Graph &gr, int origPos,
                                     int *finalPos)
{
  Graph mGr = Graph(gr);
  int n = static_cast<int>(mGr.v.size());
  int m = static_cast<int>(mGr.e.size());
  int p = mGr.p;
  //int origCr = countEdgesFromVertexCrossings (mGr, mGr.v[origPos]);
  moveVertex(&mGr, origPos, 0);

  vector<Edge> allPageE;
  for (Edge *e : mGr.v[0].neighs)
    for (int i = 0; i < p; i++)
    {
      Edge newE = Edge(0, e->getOtherEnd(0));
      newE.p = i;
      allPageE.push_back(newE);
    }
  addEdgesGraphCrossings(mGr, allPageE);

  int curDiff = 0;
  int bestDiff = n * m + 1;  // difference of the original to the best other; there are at most n edges from origPos, each crossing at most m edges
  int origDiff = 0;
  int best = 0;
  for (int j = 0; j < n; j++)
  {
    if (j > 0)
    {
      for (std::size_t ngh = 0; ngh < mGr.v[j - 1].neighs.size(); ngh++)
        curDiff -= allPageE[p * ngh + mGr.v[j - 1].neighs[ngh]->p].cr;
      //curDiff -= countEdgesFromVertexCrossings (mGr, mGr.v[j - 1]);
      removeEdgesVertexCrossings(mGr, j, allPageE);
      swapVertices(&mGr, j - 1, j);
      for (Edge &e : allPageE)
      {
        assert(e.v1 == j - 1);
        e.v1 = j;
        if (e.v2 == j)
          e.v2 = j - 1;
      }
      addEdgesVertexCrossings(mGr, j - 1, allPageE);
    }
    for (unsigned ngh = 0; ngh < mGr.v[j].neighs.size(); ngh++)
    {
      assert(mGr.v[j].neighs[ngh]->getOtherEnd(j) == allPageE[p * ngh].v2);
      int bestPage = std::min_element(allPageE.begin() + p * ngh,
                                      allPageE.begin() + p * (ngh + 1),
                                      [](const Edge &e1, const Edge &e2)
                                      { return e1.cr < e2.cr;})->p;
      mGr.v[j].neighs[ngh]->p = bestPage;
      curDiff += allPageE[p * ngh + bestPage].cr;
    }
    // curDiff += countEdgesFromVertexCrossings (mGr, mGr.v[j]);
    if (j == origPos)
      origDiff = curDiff;
    if (j != origPos && curDiff <= bestDiff)
    {
      bestDiff = curDiff;
      best = j;
    }
  }
  moveVertex(&mGr, n - 1, best);
  greedyAtVertex(&mGr, best);
  //int bestCr = countEdgesFromVertexCrossings (mGr, mGr.v[best]);

  int retval = bestDiff - origDiff;
  //int retval = bestCr - origCr;
//  cout << best << ", " << retval << endl;
  *finalPos = best;
  return retval;
}

void Tools::BaurBrandes(Graph *gr)
{
  int n = static_cast<int>(gr->v.size());
  bool improved = true;
  while (improved)
  {
    improved = false;
    for (int i = 0; i < n; i++)
    {
      int best;
      int change = findBestPositionForVertex(*gr, i, &best);
      // assert(change <= 0);
      if (change < 0)
      {
        moveVertex(gr, i, best);
        greedyAtVertex(gr, best);
        improved = true;
        //cerr << "Improved by " << change << endl;
      }
    }
  }
}

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
    {
      int origCr = countEdgeCrossings(*gr, *ed);
      int origPage = ed->p;
      int bestCr = origCr;
      int best = origPage;
      for (int pNew = 0; pNew < gr->p; pNew++)
        if (pNew != origPage)
        {
          ed->p = pNew;
          int curCr = countEdgeCrossings(*gr, *ed);
          if (curCr <= bestCr || best < 0)
          {
            if (curCr < bestCr || best < 0)
              improved = true;
            bestCr = curCr;
            best = pNew;
          }
        }
      ed->p = best;
    }
  }
}

int Tools::restartEdges(Graph *gr, int prevCr, void (*placer)(Graph *gr))
{
  Graph grBck = *gr;
  for (Edge &e : gr->e)
    e.p = -1;
  placer(gr);
  int expCr = countCrossingNumber(*gr);
  if (prevCr < expCr)
  {
    gr->loadFrom(grBck);
    return prevCr;
  }
  else
  {
    return expCr;
  }
}
