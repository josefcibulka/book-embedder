#ifndef BESTFOUND_H_
#define BESTFOUND_H_

#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#include "graph.h"

class BestFound
{
 public:
  BestFound(std::string filename, Graph &origGr)
      : filename_(filename),
        filenameBck_(filename + ".bck"),
        origGr_(origGr)
  {
    testIfBest(origGr, -1);
    betterThanInitial_ = false; // must be here, because testIfBest changes it to true
  }

  void testIfBest(const Graph &candidate, int claimedCr);

  int val() const
  {
    return val_;
  }

  const Graph &gr() const
  {
    return gr_;
  }

  bool betterThanInitial() const
  {
    return betterThanInitial_;
  }

  void restart()
  {
    val_ = -1;
    betterThanInitial_ = false;
  }

 private:
  std::string filename_ = "";
  std::string filenameBck_ = "";
  int val_ = -1;
  Graph gr_;
  Graph origGr_;
  bool betterThanInitial_ = false;

  void writeGraph(const Graph &g, std::ostream &ostr);

  void verifyGraphBadCase(std::string msg);

  void verifyGraph(const Graph &gr, int claimedCr);
};

#endif /* BESTFOUND_H_ */
