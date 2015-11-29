/**
 * This program generates complete multipartite graphs with equally-sized partitions
 * for use by book-embedder.
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
    "Exactly three arguments are required -- the number of the pages provided for the drawing, "
        "the number of vertices in each partition of the graph, and the number of partitions. "
        "Two vertices are connected iff they belong to different partitions.\n"
        "E.g. the complete bipartite graph with 20 vertices in total to be drawn in a book with 3 pages: "
        "\"gen_complete_tpartite 3 10 2\".\n";

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

  int t = -1;
  t = atoi(argv[3]);
  if (t <= 0 || t > MAXN)
    print_usage_and_exit();

  int nn = n * t;

  printf("%d\n", nn);
  printf("%d\n", p);
  for (int i = 0; i < nn; i++)
    printf("%d\n", i);

  for (int i = 0; i < nn; i++)
    for (int j = i + 1; j < nn; j++)
      if (i / n != j / n)
        printf("%d %d [0]\n", i, j);
  return 0;
}
