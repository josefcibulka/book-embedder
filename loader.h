/**
 * Functions for loading graphs in the format of the Graph Drawing 2015 challenge.
 *
 * Program for minimizing the number of crossings in a book embedding of a given
 * graph to a given number of pages.
 *
 * Author: Josef Cibulka
 * License: see the file LICENSE
 */
#ifndef BOOK_EMBEDDER_LOADER_H_
#define BOOK_EMBEDDER_LOADER_H_

#include <vector>
#include <istream>

#include "graph.h"

class Loader
{
 public:
  static void load(std::istream &input, Graph *gr);

 private:
  static std::string mygetline(std::istream &input);

  static int mygetnumber(std::istream &input);

  static Edge mygetedge(std::istream &input, std::vector<int> &whereIsVertex);

};

#endif
