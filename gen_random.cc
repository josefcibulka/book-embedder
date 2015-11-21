#include <ctime>
#include <cstdio>
#include <cstdlib>

#define MAXP 1000000
#define MAXN 1000000

void print_usage_and_exit()
{
   fprintf(
         stderr,
         "Exactly three arguments are required -- the number of pages, the number of vertices and the edge density (in percents) of the graph.\n");
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

   double prob = -1;
   prob = atof(argv[3]);
   if (prob <= 0.0 || prob > 100.0)
      print_usage_and_exit();

   printf("%d\n", n);
   printf("%d\n", p);

   int thr = prob * RAND_MAX / 100.0;

   srand(time(nullptr));
   for (int i = 0; i < n; i++)
      printf("%d\n", i);

   for (int i = 0; i < n; i++)
      for (int j = i + 1; j < n; j++)
         if (rand() < thr)
            printf("%d %d [0]\n", i, j);
   return 0;
}
