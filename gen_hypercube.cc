#include <cstdio>
#include <cstdlib>

#define MAXP 1000000
#define MAXD 30

void
print_usage_and_exit ()
{
  fprintf (
      stderr,
      "Exactly two arguments are required -- the number of pages and the number of dimensions.\n");
  exit (0);
}

int
main (int argc, char *argv[])
{
  if (argc != 3)
    print_usage_and_exit ();

  int p = -1;
  p = atoi (argv[1]);
  if (p <= 0 || p > MAXP)
    print_usage_and_exit ();

  int d = -1;
  d = atoi (argv[2]);
  if (d <= 0 || d > MAXD)
    print_usage_and_exit ();

  int n = (1 << d);

  printf ("%d\n", n);
  printf ("%d\n", p);

  for (int i = 0; i < n; i++)
    printf ("%d\n", i);

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
	  printf ("%d %d [0]\n", i, j);
      }
  return 0;
}
