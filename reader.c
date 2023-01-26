#include "reader.h"
#include "cputracker.h"

int get_nproc() {
  FILE *fp;
  char output[64];

  fp = popen("nproc", "r");
  if (fp == NULL) {
    printf("Reading number of threads failed\n");
    exit(EXIT_FAILURE);
  }

  int nproc = 0;
  if (fgets(output, sizeof(output) - 1, fp) != NULL) {
    sscanf(output, "%d", &nproc);
  }

  pclose(fp);
  return nproc;
}

struct proc_stat *get_proc_stats() {
  FILE *file = fopen("/proc/stat", "r");
  char line[1024];
  struct proc_stat *stats = malloc(nproc * sizeof(struct proc_stat));

  for (int thread = 0; thread < nproc; thread++) {
    fgets(line, sizeof(line), file);
    // assert(strncmp(line, "cpu", 3) == 0);
    if (strncmp(line, "cpu", 3) != 0) {
      printf("Reading thread info failed\n");
      exit(EXIT_FAILURE);
    }
    sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           stats[thread].name, &stats[thread].user, &stats[thread].nice,
           &stats[thread].system, &stats[thread].idle, &stats[thread].iowait,
           &stats[thread].irq, &stats[thread].softirq, &stats[thread].steal,
           &stats[thread].guest, &stats[thread].guest_nice);
  }
  return stats;
}

int print_stats() {
  struct proc_stat *stats = get_proc_stats();

  for (int i = 0; i < nproc; i++) {
    printf("%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", stats[i].name,
           stats[i].user, stats[i].nice, stats[i].system, stats[i].idle,
           stats[i].iowait, stats[i].irq, stats[i].softirq, stats[i].steal,
           stats[i].guest, stats[i].guest_nice);
  }

  free(stats);
  return 0;
}