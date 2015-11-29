/**
 * This program generates circulant graphs for use by book-embedder.
 *
 * Program for minimizing the number of crossings in a book embedding of a given
 * graph to a given number of pages.
 *
 * Author: Josef Cibulka
 * License: see the file LICENSE
 */

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#define MAXP 1000000
#define MAXN 1000000

const std::string usage =
    "Exactly three arguments are required -- the number of the pages provided for the drawing, "
        "the number of vertices of the graph, and a comma-separated list of the edge lengths. "
        "Two vertices u,v are connected iff abs(u-v) mod n is from the list of edge lengths."
        "No spaces in the list of edge lengths.\n"
        "E.g. \"gen_circulant 2 10 1,2,3\".\n";

void print_usage_and_exit()
{
  fprintf(stderr, usage.c_str());
  exit(0);
}

int main(int argc, char *argv[])
{
  if (argc != 4)
    print_usage_and_exit();

  int p = -1;
  p = atoi(argv[1]);
  if (p <= 0 || p > MAXP)
    print_usage_and_exit();

  int n = -1;
  n = atoi(argv[2]);
  if (n <= 0 || n > MAXN)
    print_usage_and_exit();

  std::string lString(argv[3]);
  std::istringstream istr(lString);

  std::vector<int> lengths;

  int i;
  while (istr >> i)
  {
    lengths.push_back(i);

    if (istr.peek() == ',')
      istr.ignore();
  }

  printf("%d\n", n);
  printf("%d\n", p);
  for (int i = 0; i < n; i++)
    printf("%d\n", i);

  for (int i = 0; i < n; i++)
    for (int l : lengths)
      printf("%d %d [0]\n", i, (i + l) % n);
  return 0;
}
