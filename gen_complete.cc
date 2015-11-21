#include <cstdio>
#include <cstdlib>

#define MAXP 1000000
#define MAXN 1000000

void print_usage_and_exit()
{
  fprintf(stderr, "Exactly two arguments are required -- the number of pages and the size of the graph\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  if(argc!=3)
    print_usage_and_exit();

  int p = -1;
  p = atoi(argv[1]);
  if(p<=0 || p>MAXP)
    print_usage_and_exit();
  
  int n = -1;
  n = atoi(argv[2]);
  if(n<=0 || n>MAXN)
    print_usage_and_exit();

  printf("%d\n", n);
  printf("%d\n", p);
  for(int i=0; i<n; i++)
    printf("%d\n", i);
  
  for(int i=0; i<n; i++)
    for(int j=i+1; j<n; j++)
      printf("%d %d [0]\n", i, j);
  return 0;
}
