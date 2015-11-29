/**
 * Functions for loading graphs in the format of the Graph Drawing 2015 challenge.
 *
 * Program for minimizing the number of crossings in a book embedding of a given
 * graph to a given number of pages.
 *
 * Author: Josef Cibulka
 * License: see the file LICENSE
 */

#include <istream>

class Graph;

void load(std::istream &input, Graph *gr);
