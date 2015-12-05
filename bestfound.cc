#include "bestfound.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

#include "tools.h"

void BestFound::writeGraph(const Graph &g, std::ostream &ostr)
{
  ostr << g.v.size() << endl;
  ostr << g.p << endl;
  for (const Vertex &v : g.v)
    ostr << v.id << endl;
  for (const Edge &e : g.e)
    ostr << g.v[e.v1].id << " " << g.v[e.v2].id << " [" << e.p << "]" << endl;
}

void BestFound::verifyGraphBadCase(string msg)
{
  cerr << "The graph is bad!!! " << msg << endl;
  exit(0);
}

void BestFound::verifyGraph(const Graph &gr, int claimedCr)
{
  if (Tools::countCrossingNumber(gr) != claimedCr)
    verifyGraphBadCase("Number of crossings differs from the claimend value.");
  if (origGr_.v.size() != gr.v.size())
    verifyGraphBadCase("Number of vertices changed.");
  if (origGr_.e.size() != gr.e.size())
    verifyGraphBadCase("Number of edges changed.");
  int n = static_cast<int>(gr.v.size());
  int m = static_cast<int>(gr.e.size());
  if (origGr_.p != gr.p)
    verifyGraphBadCase("Numbers of pages changed.");

  vector<bool> used(n, false);
  for (const Vertex &ver : gr.v)
  {
    if (ver.id < 0 || ver.id >= n || used[ver.id])
      verifyGraphBadCase(
          "Bad or duplicate vertex id " + std::to_string(ver.id) + ".");
    used[ver.id] = true;
  }
  for (int i = 0; i < m; i++)
  {
    if (gr.v[gr.e[i].v1].id != origGr_.v[origGr_.e[i].v1].id)
    {
      writeGraph(origGr_, cout);
      writeGraph(gr, cout);
      verifyGraphBadCase(
          "An edge changed; the first vertex of edge " + std::to_string(i));
    }
    if (gr.v[gr.e[i].v2].id != origGr_.v[origGr_.e[i].v2].id)
    {
      writeGraph(origGr_, cout);
      writeGraph(gr, cout);
      verifyGraphBadCase(
          "An edge changed; the second vertex of edge " + std::to_string(i));
    }
    if (gr.e[i].p < 0 || gr.e[i].p >= gr.p)
      verifyGraphBadCase("Edge page is out of range.");
  }
}

void BestFound::testIfBest(const Graph &candidate, int claimedCr)
{
  if (claimedCr >= val_)
    return;

  verifyGraph(candidate, claimedCr);
  val_ = claimedCr;
  gr_.loadFrom(candidate);

  cerr << "Writing graph with: " << claimedCr << " crossings." << endl;
  std::ofstream ostr;
  ostr.open(filename_.c_str(), std::ios_base::out);
  if (!ostr.is_open())
    return;
  writeGraph(candidate, ostr);
  ostr.close();
}
