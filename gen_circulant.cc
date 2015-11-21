#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#define MAXP 1000000
#define MAXN 1000000

void print_usage_and_exit()
{
   fprintf(
         stderr,
         "Exactly three arguments are required -- the number of pages, the number of vertices and a comma-separated list of the edge lengths. No spaces in the list of ege lengths.\n");
   fprintf(stderr, "E.g. \"gen_cirulant 2 10 1,2,3\n");
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
   std::stringstream istr(lString);

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
         printf("%d %d [0]\n", i, (i+l)%n);
   return 0;
}
