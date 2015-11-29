/**
 * This program generates complete graphs for use by book-embedder.
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
#define MAXN 1000000

const std::string usage =
    "Exactly two arguments are required -- the number of the pages provided for the drawing, "
        "and the number of vertices of the graph.\n"
        "E.g. the complete graph with 10 vertices to be drawn in a book with 5 pages (there is a crossing-free drawing): "
        "\"gen_complete 5 10\".\n";

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

  int n = -1;
  n = atoi(argv[2]);
  if (n <= 0 || n > MAXN)
    print_usage_and_exit();

  printf("%d\n", n);
  printf("%d\n", p);
  for (int i = 0; i < n; i++)
    printf("%d\n", i);

  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
      printf("%d %d [0]\n", i, j);
  return 0;
}
