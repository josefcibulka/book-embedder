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

#define MY_HUGE 1000000000

class BestFound
{
 public:
  BestFound(std::string filename, Graph &origGr)
      : filename_(filename),
        origGr_(origGr)
  {
  }

  void testIfBest(const Graph &candidate, int claimedCr);

  int val() const
  {
    return val_;
  }

 private:
  std::string filename_ = "";
  int val_ = MY_HUGE;
  Graph gr_;
  Graph origGr_;

  void writeGraph(const Graph &g, std::ostream &ostr);

  void verifyGraphBadCase(std::string msg);

  void verifyGraph(const Graph &gr, int claimedCr);
};

#endif /* BESTFOUND_H_ */
