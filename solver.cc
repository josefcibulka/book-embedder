#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#include "graph.h"
#include "loader.h"
#include "bestfound.h"
#include "tools.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

std::random_device rd;
std::mt19937 mt(rd());

void BaurBrandes(Graph *gr, BestFound *best)
{
  int n = static_cast<int>(gr->v.size());
  bool improved = true;
  while (improved)
  {
    improved = false;
    for (int i = 0; i < n; i++)
    {
      int bestPos;
      int change = Tools::findBestPositionForVertex(*gr, i, &bestPos);
      if (change < 0)
      {
        Tools::moveVertex(gr, i, bestPos);
        Tools::greedyAtVertex(gr, bestPos);
        improved = true;
      }
    }
    if (improved)
      best->testIfBest(*gr, -1);
  }
}

int BBGreedy(Graph *gr, BestFound *best)
{
  while (true)
  {
    int oldCr = Tools::countCrossingNumber(*gr);
    BaurBrandes(gr, best);
    int newCr = Tools::countCrossingNumber(*gr);

    best->testIfBest(*gr, newCr);

    Tools::greedyPages(gr);
    newCr = Tools::countCrossingNumber(*gr);
    best->testIfBest(*gr, newCr);
    newCr = Tools::restartEdges(gr, newCr, Tools::lenPages);
    newCr = Tools::restartEdges(gr, newCr, Tools::greedyPages);

    if (newCr == oldCr)
      break;
    best->testIfBest(*gr, newCr);
  }
  int finalCr = Tools::countCrossingNumber(*gr);
  cout << endl << "BBGreedy: " << finalCr << endl;
  return finalCr;
}

int GreedyBB(Graph *gr, BestFound *best)
{
  while (true)
  {
    int oldCr = Tools::countCrossingNumber(*gr);
    Tools::greedyPages(gr);
    int newCr = Tools::countCrossingNumber(*gr);
    best->testIfBest(*gr, newCr);

    newCr = Tools::restartEdges(gr, newCr, Tools::lenPages);
    newCr = Tools::restartEdges(gr, newCr, Tools::greedyPages);
    best->testIfBest(*gr, newCr);

    BaurBrandes(gr, best);
    newCr = Tools::countCrossingNumber(*gr);
    if (newCr == oldCr)
      break;

    best->testIfBest(*gr, newCr);
  }
  int finalCr = Tools::countCrossingNumber(*gr);
  cout << endl << "GreedyBB: " << finalCr << endl;
  return finalCr;
}

int simAnneal(Graph *gr, double t0, BestFound *best)
{
//  double t = t0;
  double t1 = 0.2;
  int endIter = 1000;
  int begIter = endIter / 50;
//  double alpha = 0.999;
  int m = gr->e.size();
  int n = gr->v.size();
  std::uniform_int_distribution<int> vertexDistrib(0, n - 1);
  std::uniform_int_distribution<int> edgeDistrib(0, m - 1);
  std::uniform_int_distribution<int> pageDistrib(0, gr->p - 2);
  std::uniform_real_distribution<double> zeroOneDistrib(0, 1);

  int r1 = m;
  int r2 = sqrt(n) * n;  // 10 * n;  //n * n;
  int r3 = n;
  int r4 = n / 4 + 1;
  int crCnt = Tools::countCrossingNumber(*gr);
  BestFound SABest("", *gr);
  SABest.restart();
  for (int iter = begIter; iter < endIter && crCnt > 0; iter++)
  //while (t > t1 && crCnt > 0)
  {
    double t = t0
        + (1 / log(begIter) - 1 / log(iter)) * (t1 - t0)
            / (1 / log(begIter) - 1 / log(endIter));
    for (int c = 0; c < r1; c++)
    {
      Edge *ed = &(gr->e[edgeDistrib(mt)]);
      int crDiff = -Tools::countEdgeCrossings(*gr, *ed);
      int origP = ed->p;
      int p = pageDistrib(mt);
      if (p >= origP)
        p++;
      ed->p = p;
      crDiff += Tools::countEdgeCrossings(*gr, *ed);
      if (crDiff > 0 && zeroOneDistrib(mt) >= ::exp(-crDiff / t))
      {
        ed->p = origP;
      }
      else
      {
        crCnt += crDiff;
        best->testIfBest(*gr, crCnt);
        SABest.testIfBest(*gr, crCnt);
      }
    }
    for (int c = 0; c < r2; c++)
    {
      int v1 = vertexDistrib(mt);
      if (v1 == n - 1)
        continue;
      int crDiff = Tools::countCrossingChangeIfNeighborsSwapped(*gr, v1);
      if (crDiff <= 0 || zeroOneDistrib(mt) < ::exp(-crDiff / t))
      {
        // do the change
        Tools::swapVertices(gr, v1, v1 + 1);
        crCnt += crDiff;
        best->testIfBest(*gr, crCnt);
        SABest.testIfBest(*gr, crCnt);
      }

    }
    for (int c = 0; c < r3; c++)
    {
      int v1 = vertexDistrib(mt);
      int v2 = vertexDistrib(mt);
      if (v1 == v2)
        continue;
      int crDiff = -Tools::countEdgesFromVertexCrossings(*gr, gr->v[v1]);
      vector<Edge> edge_bck = gr->e;
      Tools::moveVertex(gr, v1, v2);
      Tools::greedyAtVertex(gr, v2);
      crDiff += Tools::countEdgesFromVertexCrossings(*gr, gr->v[v2]);
      if (crDiff > 0 && zeroOneDistrib(mt) >= ::exp(-crDiff / t))
      {
        //restore to original
        Tools::moveVertex(gr, v2, v1);
        for (unsigned i = 0; i < edge_bck.size(); i++)
          gr->e[i].p = edge_bck[i].p;
      }
      else
      {
        crCnt += crDiff;
        best->testIfBest(*gr, crCnt);
        SABest.testIfBest(*gr, crCnt);
      }
    }
    for (int c = 0; c < r4; c++)
    {
      int v1 = vertexDistrib(mt);
      int v2 = 0;
      int crDiff = Tools::findBestPositionForVertex(*gr, v1, &v2);
      if (crDiff <= 0 || zeroOneDistrib(mt) < ::exp(-crDiff / t))
      {
        // do the change
        Tools::moveVertex(gr, v1, v2);
        Tools::greedyAtVertex(gr, v2);
        crCnt += crDiff;
        assert(crCnt == Tools::countCrossingNumber(*gr));
        best->testIfBest(*gr, crCnt);
        SABest.testIfBest(*gr, crCnt);
      }

    }
    //t *= alpha;
  }
  if (SABest.betterThanInitial())
  {
    gr->loadFrom(SABest.gr());
    int finalCr = SABest.val();
    assert(finalCr <= crCnt);
    assert(finalCr == Tools::countCrossingNumber(*gr));
    cout << endl << "SimAnneal before BBgreedy: Last value: " << crCnt
         << ", best value (will be used): " << finalCr << endl;
    crCnt = finalCr;
  }
  else
  {
    cout << endl << "SimAnneal before BBgreedy: Last value (will be used): "
         << crCnt << ", best value is the initial (" << SABest.val() << ")"
         << endl;
  }
  crCnt = BBGreedy(gr, best);
  cout << "SimAnneal: " << crCnt << endl;
  return crCnt;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "One parameter is required - the output filename." << endl;
    return 0;
  }

  string filename = string(argv[1]);
  Graph origGr;

  Loader::load(std::cin, &origGr);
  cout << "Loaded graph has " << Tools::countCrossingNumber(origGr)
       << " crossings." << endl;

  BestFound best(filename, origGr);

  int n = static_cast<int>(origGr.v.size());

  Graph graphGBB(origGr);
  int valGBB = GreedyBB(&graphGBB, &best);
  best.testIfBest(graphGBB, valGBB);

  Graph graphBBG(origGr);
  int valBBG = BBGreedy(&graphBBG, &best);
  best.testIfBest(graphBBG, valBBG);

  // In every iteration, the starting solution is changed - first graphGBB and graphBBG
  // are used, afterwards, a random vertex ordering is used.
  // Next, the simmulated annealing with high initial temperature starts, followed
  // by another with a lower initial temperature.
  int iterCnt = 5;
  for (int i = 0; i < iterCnt; i++)
  {
    cout << "---------------------------------------" << endl;
    Graph graphSA;
    if (i == 0)
      graphSA.loadFrom(graphGBB);
    else if (i == 1)
      graphSA.loadFrom(graphBBG);
    else if (i % 5 == 4)
      graphSA.loadFrom(best.gr());
    else  // random shuffle
    {
      graphSA.loadFrom(origGr);
      for (int j = 0; j < 10 * n; j++)
      {
        std::uniform_int_distribution<int> vertexDistrib(0, n - 1);
        int v1 = vertexDistrib(mt);
        int v2 = vertexDistrib(mt);
        if (v1 == v2)
          continue;
        Tools::moveVertex(&graphSA, v1, v2);
      }
      int crTmp = Tools::countCrossingNumber(graphSA);
      Tools::restartEdges(&graphSA, crTmp, Tools::lenPages);
    }

    int valSA = simAnneal(&graphSA, 64, &best);
    best.testIfBest(graphSA, valSA);

    valSA = simAnneal(&graphSA, 8, &best);
    best.testIfBest(graphSA, valSA);
  }
  cout << "Result is: " << best.val() << endl;
}
