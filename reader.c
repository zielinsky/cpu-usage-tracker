#include "reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_nproc() {
  FILE *fp;
  char output[64];

  fp = popen("nproc", "r");
  if (fp == NULL) {
    printf("Reading number of threads failed\n");
    exit(1);
  }

  int nproc = 0;
  if (fgets(output, sizeof(output) - 1, fp) != NULL) {
    sscanf(output, "%d", &nproc);
  }

  pclose(fp);
  return nproc;
}

int main() {
  FILE *file = fopen("/proc/stat", "r");
  char line[1024];
  struct proc_stat stats[get_nproc() + 1];
  int thread_count = 0;

  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "cpu", 3) == 0) {
      sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
             stats[thread_count].name, &stats[thread_count].user,
             &stats[thread_count].nice, &stats[thread_count].system,
             &stats[thread_count].idle, &stats[thread_count].iowait,
             &stats[thread_count].irq, &stats[thread_count].softirq,
             &stats[thread_count].steal, &stats[thread_count].guest,
             &stats[thread_count].guest_nice);
      thread_count++;
    }
  }

  for (int i = 0; i < thread_count; i++) {
    printf("%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", stats[i].name,
           stats[i].user, stats[i].nice, stats[i].system, stats[i].idle,
           stats[i].iowait, stats[i].irq, stats[i].softirq, stats[i].steal,
           stats[i].guest, stats[i].guest_nice);
  }

  fclose(file);
  return 0;
}