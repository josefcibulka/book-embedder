/**
 * This program generates the hypercube graphs for use by book-embedder.
 *
 * Program for minimizing the number of crossings in a book embedding of a given
 * graph to a given number of pages.
 *
 * Author: Josef Cibulka
 * License: see the file LICENSE
 */

#include <cstdio>
#include <cstdlib>
#include <string>

#define MAXP 1000000
#define MAXD 30

const std::string usage =
    "Exactly two arguments are required -- the number of the pages provided for the drawing, "
        "and the number of dimensions of the hypercube. A d-dimensional hypercube has 2^d vertices.\n"
        "E.g. the 4-dimensional hypercube to be drawn in a book with 3 pages (there is a crossing-free drawing): "
        "\"gen_hypercube 3 4\".\n";

void print_usage_and_exit()
{
  fprintf(stderr, usage.c_str());
  exit(0);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
    print_usage_and_exit();

  int p = -1;
  p = atoi(argv[1]);
  if (p <= 0 || p > MAXP)
    print_usage_and_exit();

  int d = -1;
  d = atoi(argv[2]);
  if (d <= 0 || d > MAXD)
    print_usage_and_exit();

  int n = (1 << d);

  printf("%d\n", n);
  printf("%d\n", p);

  for (int i = 0; i < n; i++)
    printf("%d\n", i);

  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
    {
      int ixorj = (i ^ j);
      int bits = 0;
      while (ixorj > 0)
      {
        bits += (ixorj & 1);
        ixorj >>= 1;
      }
      if (bits == 1)
        printf("%d %d [0]\n", i, j);
    }
  return 0;
}
