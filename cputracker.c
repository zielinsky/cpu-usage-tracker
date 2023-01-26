#include "cputracker.h"
#include "reader.h"
#include <stdlib.h>

int nproc = 0;

int main() {
  if (get_nproc(&nproc) == -1)
    exit(EXIT_FAILURE);
  nproc++;
  print_stats();
  return 0;
}