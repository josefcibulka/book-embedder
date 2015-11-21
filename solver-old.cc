#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#include "graph.h"
#include "loader.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

int pageCnt;
std::random_device rd;
std::mt19937 mt (rd ());

void
writeGraph (const Graph &g, std::ostream &ostr)
{
  ostr << g.v.size () << endl;
  ostr << g.p << endl;
  for (const Vertex &v : g.v)
    ostr << v.id << endl;
  for (const Edge &e : g.e)
    ostr << g.v[e.v1].id << " " << g.v[e.v2].id << " [" << e.p << "]" << endl;
}

/**
 * O(m) (less if the edge is short)
 */
int
countEdgeCrossings (const Graph &gr, const Edge &ed)
{
  int result = 0;

  int v1 = std::min (ed.v1, ed.v2);
  int v2 = std::max (ed.v1, ed.v2);

  for (int id2 = v1 + 1; id2 < v2; id2++)
    for (const Edge *ed2 : gr.v[id2].neighs)
      {
	if (ed.p != ed2->p)
	  continue;
	int e2V2 = ed2->getOtherEnd (id2);
	if (e2V2 < v1 || e2V2 > v2)
	  result++;
      }
  return result;
}

/**
 * O(deg(ver)*m) ~ O(m^2/n) (less if the edges are short)
 */
int
countEdgesFromVertexCrossings (const Graph &gr, const Vertex &ver)
{
  int result = 0;
  for (const Edge *ed : ver.neighs)
    result += countEdgeCrossings (gr, *ed);
  return result;
}

/**
 * O(m^2) (one m is smaller if the edges are short)
 */
int
countCrossingNumber (const Graph &gr)
{
  int result = 0;
  for (const Edge &ed : gr.e)
    result += countEdgeCrossings (gr, ed);
  result >>= 1;
  return result;
}

/**
 * Counts crossings between pairs of edges, where one edge has v1 as one of its end-points
 * and the other edge has v1+1 as one of its end-points.
 * O(deg(v1)*deg(v1+1)) ~ O(m^2/n^2)
 */
int
countEdgesFromNeighborsCrossings (const Graph &gr, int v1)
{
  int result = 0;
  assert(v1 + 1 < static_cast<int> (gr.v.size ()));
  const Vertex &ver1 = gr.v[v1];
  const Vertex &ver2 = gr.v[v1 + 1];
  for (const Edge *ed1 : ver1.neighs)
    for (const Edge *ed2 : ver2.neighs)
      {
	if (ed1->p != ed2->p)
	  continue;
	int ed1v2 = ed1->getOtherEnd (v1);
	int ed2v2 = ed2->getOtherEnd (v1 + 1);
	if (ed1v2 > v1 + 1 && (ed2v2 < v1 || ed2v2 > ed1v2))
	  result++;
	else if (ed1v2 < v1 && ed2v2 < v1 && ed2v2 > ed1v2)
	  result++;
      }
  return result;
}

/**
 * Move a single vertex from vOld to vNew.
 */
void
moveVertex (Graph *gr, int vOld, int vNew)
{
  //cerr << "moving " << vOld << " -> " << vNew << endl;
  if (vOld == vNew)
    return;
  vector<int> newPos;
  int n = static_cast<int> (gr->v.size ());
  for (int i = 0; i < n; i++)
    {
      int nPos = i;
      if (i > vOld)
	nPos--;  // vertices after the old position move back
      if (nPos >= vNew)
	nPos++;  // vertices on or after the new position move forward
      if (i == vOld)
	nPos = vNew;  // the moved vertex
      newPos.push_back (nPos);
    }
  vector<Vertex> newVerVector (n, Vertex (-1));
  for (int i = 0; i < n; i++)
    newVerVector[newPos[i]] = std::move (gr->v[i]);
  gr->v = std::move (newVerVector);

  for (Edge &ed : gr->e)
    {
      ed.v1 = newPos[ed.v1];
      ed.v2 = newPos[ed.v2];
    }

  for (Vertex &ver : gr->v)
    if (ver.id == -1)
      {
	writeGraph (*gr, cout);
	exit (0);
      }
}

/**
 * Swap vertices vA and vB.
 */
void
swapVertices (Graph *gr, int vA, int vB)
{
  for (Edge *ed : gr->v[vA].neighs)
    {
      if (ed->v1 == vA && ed->v2 == vB)
	std::swap (ed->v1, ed->v2);
      else if (ed->v1 == vB && ed->v2 == vA)
	std::swap (ed->v1, ed->v2);
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
  std::swap (gr->v[vA], gr->v[vB]);
}

void
BaurBrandes (Graph *gr)
{
  int n = static_cast<int> (gr->v.size ());
  bool improved = true;
  while (improved)
    {
      improved = false;
      for (int i = 0; i < n; i++)
	{
	  moveVertex (gr, i, 0);
	  int curDiff = 0;
	  int origDiff = 0;
	  int bestDiff = 0;
	  int best = 0;
	  for (int j = 1; j < n; j++)
	    {
	      curDiff -= countEdgesFromNeighborsCrossings (*gr, j - 1);
	      swapVertices (gr, j - 1, j);
	      curDiff += countEdgesFromNeighborsCrossings (*gr, j - 1);
	      if (j == i)
		origDiff = curDiff;
	      if (curDiff <= bestDiff)
		{
		  bestDiff = curDiff;
		  best = j;
		}
	    }
	  if (bestDiff < origDiff)
	    {
	      improved = true;
	      //cerr << "Improved by " << bestDiff-origDiff << endl;
	    }
	  moveVertex (gr, n - 1, best);
	}
    }
}

bool
greedyEdgePage (Graph *gr, Edge *ed)
{
  bool improved = false;
  int origCr = countEdgeCrossings (*gr, *ed);
  int origPage = ed->p;
  int bestCr = origCr;
  int best = origPage;
  for (int pNew = 0; pNew < gr->p; pNew++)
    if (pNew != origPage)
      {
	ed->p = pNew;
	int curCr = countEdgeCrossings (*gr, *ed);
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

void
greedyAtVertex (Graph *gr, int v)
{
  for (Edge *ed : gr->v[v].neighs)
    greedyEdgePage (gr, ed);
}

void
greedyPages (Graph *gr)
{
  bool improved = true;
  while (improved)
    {
      improved = false;
      for (Edge &ed : gr->e)
	if (greedyEdgePage (gr, &ed))
	  improved = true;
    }
}

void
lenPages (Graph *gr)
{
  vector<Edge *> eSorted;
  for (Edge &e : gr->e)
    eSorted.push_back (&e);
  std::sort (eSorted.begin (), eSorted.end (), EdgeLengthComparer ());
  bool improved = true;
  while (improved)
    {
      improved = false;
      for (Edge *ed : eSorted)
	{
	  int origCr = countEdgeCrossings (*gr, *ed);
	  int origPage = ed->p;
	  int bestCr = origCr;
	  int best = origPage;
	  for (int pNew = 0; pNew < gr->p; pNew++)
	    if (pNew != origPage)
	      {
		ed->p = pNew;
		int curCr = countEdgeCrossings (*gr, *ed);
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

int
restartEdges (Graph *gr, int prevCr, void
(*placer) (Graph *gr))
{
  Graph grBck = *gr;
  for (Edge &e : gr->e)
    e.p = -1;
  placer (gr);
  int expCr = countCrossingNumber (*gr);
  if (prevCr < expCr)
    {
      gr->loadFrom (grBck);
      return prevCr;
    }
  else
    {
      return expCr;
    }
}

int
BBGreedy (Graph *gr)
{
  while (true)
    {
      int oldCr = countCrossingNumber (*gr);
//      cerr << "current: " << oldCr << endl;
      BaurBrandes (gr);
//      cerr << "current: " << countCrossingNumber(*gr) << endl;
      greedyPages (gr);
      int newCr = countCrossingNumber (*gr);
      newCr = restartEdges (gr, newCr, lenPages);
      newCr = restartEdges (gr, newCr, greedyPages);
      if (newCr == oldCr)
	break;
    }
  int finalCr = countCrossingNumber (*gr);
  cerr << "BBGreedy: " << finalCr << endl;
  return finalCr;
}

int
GreedyBB (Graph *gr)
{
  while (true)
    {
      int oldCr = countCrossingNumber (*gr);
//      cerr << "current: " << oldCr << endl;
      greedyPages (gr);
      int newCr = countCrossingNumber (*gr);
      newCr = restartEdges (gr, newCr, lenPages);
      newCr = restartEdges (gr, newCr, greedyPages);
//      cerr << "current: " << countCrossingNumber(*gr) << endl;
      BaurBrandes (gr);
      newCr = countCrossingNumber (*gr);
      if (newCr == oldCr)
	break;
    }
  int finalCr = countCrossingNumber (*gr);
  cerr << "GreedyBB: " << finalCr << endl;
  return finalCr;
}

int
simAnneal (Graph *gr, double t)
{
  double t1 = 0.2;
  double alpha = 0.999;
  int m = gr->e.size ();
  int n = gr->v.size ();
  std::uniform_int_distribution<int> vertexDistrib (0, n - 1);
  std::uniform_int_distribution<int> edgeDistrib (0, m - 1);
  std::uniform_int_distribution<int> pageDistrib (0, gr->p - 2);
  std::uniform_real_distribution<double> zeroOneDistrib (0, 1);

  int r1 = m;
  int r2 = 0; //n * n;
  int r3 = n;
  int crCnt = countCrossingNumber (*gr);
  while (t > t1 && crCnt > 0)
    {
      for (int c = 0; c < r1; c++)
	{
	  Edge *ed = &(gr->e[edgeDistrib (mt)]);
	  int crDiff = -countEdgeCrossings (*gr, *ed);
	  int origP = ed->p;
	  int p = pageDistrib (mt);
	  if (p >= origP)
	    p++;
	  ed->p = p;
	  crDiff += countEdgeCrossings (*gr, *ed);
	  if (crDiff > 0 && zeroOneDistrib (mt) >= ::exp (-crDiff / t))
	    ed->p = origP;
	}
      for (int c = 0; c < r2; c++)
	{
	  int v1 = vertexDistrib (mt);
	  if (v1 == n - 1)
	    continue;
	  int crDiff = -countEdgesFromNeighborsCrossings (*gr, v1);
	  swapVertices (gr, v1, v1 + 1);
	  crDiff += countEdgesFromNeighborsCrossings (*gr, v1);
	  if (crDiff > 0 && zeroOneDistrib (mt) >= ::exp (-crDiff / t))
	    swapVertices (gr, v1, v1 + 1);
	}
      for (int c = 0; c < r3; c++)
	{
	  int v1 = vertexDistrib (mt);
	  int v2 = vertexDistrib (mt);
	  if (v1 == v2)
	    continue;
	  int crDiff = -countEdgesFromVertexCrossings (*gr, gr->v[v1]);
	  vector<Edge> edge_bck = gr->e;
	  moveVertex (gr, v1, v2);
	  greedyAtVertex (gr, v2);
	  crDiff += countEdgesFromVertexCrossings (*gr, gr->v[v2]);
	  if (crDiff > 0 && zeroOneDistrib (mt) >= ::exp (-crDiff / t))
	    {
	      //restore to original
	      moveVertex (gr, v2, v1);
	      for (unsigned i = 0; i < edge_bck.size (); i++)
		gr->e[i].p = edge_bck[i].p;
	    }
	}
      t *= alpha;
    }
  int finalCr = countCrossingNumber (*gr);
  cerr << "SimAnneal before greedy: " << finalCr << endl;
  finalCr = BBGreedy (gr);
  cerr << "SimAnneal: " << finalCr << endl;
  return finalCr;
}

void
verifyGraphBadCase (string msg)
{
  cerr << "The graph is bad!!! " << msg << endl;
  exit (0);
}

void
verifyGraph (int claimedCr, const Graph &origGr, const Graph &gr)
{
  if(countCrossingNumber(gr) != claimedCr)
    verifyGraphBadCase ("Number of crossings differs from the claimend value.");
  if (origGr.v.size () != gr.v.size ())
    verifyGraphBadCase ("Number of vertices changed.");
  if (origGr.e.size () != gr.e.size ())
    verifyGraphBadCase ("Number of edges changed.");
  int n = static_cast<int> (gr.v.size ());
  int m = static_cast<int> (gr.e.size ());
  if (origGr.p != gr.p)
    verifyGraphBadCase ("Numbers of pages changed.");

  vector<bool> used (n, false);
  for (const Vertex &ver : gr.v)
    {
      if (ver.id < 0 || ver.id >= n || used[ver.id])
	verifyGraphBadCase (
	    "Bad or duplicate vertex id " + std::to_string (ver.id) + ".");
      used[ver.id] = true;
    }
  for (int i = 0; i < m; i++)
    {
      if (gr.v[gr.e[i].v1].id != origGr.v[origGr.e[i].v1].id)
	{
	  writeGraph (origGr, cout);
	  writeGraph (gr, cout);
	  verifyGraphBadCase (
	      "An edge changed; the first vertex of edge "
		  + std::to_string (i));
	}
      if (gr.v[gr.e[i].v2].id != origGr.v[origGr.e[i].v2].id)
	{
	  writeGraph (origGr, cout);
	  writeGraph (gr, cout);
	  verifyGraphBadCase (
	      "An edge changed; the second vertex of edge "
		  + std::to_string (i));
	}
      if (gr.e[i].p < 0 || gr.e[i].p >= gr.p)
	verifyGraphBadCase ("Edge page is out of range.");
    }
}

void
writeGraphToFile (int bestVal, const Graph &origGr, const Graph &bestGr,
		  string filename)
{
  cerr << "Writing graph with: " << bestVal << " crossings." << endl;
  verifyGraph (bestVal, origGr, bestGr);
  std::ofstream ostr;
  ostr.open (filename.c_str (), std::ios_base::out);
  if (!ostr.is_open ())
    return;
  writeGraph (bestGr, ostr);
  ostr.close ();
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      cerr << "One parameter is required - the output filename." << endl;
      return 0;
    }

  string filename (argv[1]);

  Graph graph;
  load (std::cin, &graph);
  Graph bestGr;

  Graph graphBBG (graph);
  int valBBG = BBGreedy (&graphBBG);
  bestGr.loadFrom (graphBBG);
  int bestVal = valBBG;
  writeGraphToFile (bestVal, graph, bestGr, filename);

  Graph graphGBB (graph);
  int valGBB = GreedyBB (&graphGBB);
  if (valGBB < bestVal)
    {
      bestGr.loadFrom (graphGBB);
      bestVal = valGBB;
      writeGraphToFile (bestVal, graph, bestGr, filename);
    }

  Graph graphSA (graphGBB);
  int valSA = simAnneal (&graphSA, 2);
  if (valSA < bestVal)
    {
      bestGr.loadFrom (graphSA);
      bestVal = valSA;
      writeGraphToFile (bestVal, graph, bestGr, filename);
    }

  Graph graphSA2 (graphBBG);
  int valSA2 = simAnneal (&graphSA2, 2);
  if (valSA2 < bestVal)
    {
      bestGr.loadFrom (graphSA2);
      bestVal = valSA2;
      writeGraphToFile (bestVal, graph, bestGr, filename);
    }
}
