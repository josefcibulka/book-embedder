#include <cstdio>
#include <cstdlib>

#define MAXP 1000000
#define MAXN 1000000

void print_usage_and_exit()
{
  fprintf(stderr, "Exactly three arguments are required -- the number of pages, the size of each parition and the number of partitions\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  if(argc!=4)
    print_usage_and_exit();

  int p = -1;
  p = atoi(argv[1]);
  if(p<=0 || p>MAXP)
    print_usage_and_exit();
  
  int n = -1;
  n = atoi(argv[2]);
  if(n<=0 || n>MAXN)
    print_usage_and_exit();

  int t = -1;
  t = atoi(argv[3]);
  if(t<=0 || t>MAXN)
    print_usage_and_exit();

  int nn = n*t;

  printf("%d\n", nn);
  printf("%d\n", p);
  for(int i=0; i<nn; i++)
    printf("%d\n", i);
  
  for(int i=0; i<nn; i++)
    for(int j=i+1; j<nn; j++)
       if(i/n != j/n)
          printf("%d %d [0]\n", i, j);
  return 0;
}
