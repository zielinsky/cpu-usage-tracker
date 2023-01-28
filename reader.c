#include "reader.h"
#include "cputracker.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

struct proc_stat *get_proc_stats() {
  FILE *file = fopen("/proc/stat", "r");
  char line[1024];
  struct proc_stat *stats = malloc(g_nproc * sizeof(struct proc_stat));
  for (int thread = 0; thread < g_nproc; thread++) {
    fgets(line, sizeof(line), file);
    // assert(strncmp(line, "cpu", 3) == 0);
    if (strncmp(line, "cpu", 3) != 0) {
      perror("Reading thread info failed");
      fclose(file);
      free(stats);
      return NULL;
    }

    sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           stats[thread].name, &stats[thread].user, &stats[thread].nice,
           &stats[thread].system, &stats[thread].idle, &stats[thread].iowait,
           &stats[thread].irq, &stats[thread].softirq, &stats[thread].steal,
           &stats[thread].guest, &stats[thread].guest_nice);
  }
  fclose(file);
  return stats;
}

void reader() {
  struct proc_stat *stats = NULL;
  while (1) {
    if ((stats = get_proc_stats()) == NULL) {
      return;
    }

    sem_wait(&g_leftSpaceSemaphore);

    pthread_mutex_lock(&g_bufferMutex);

    put_item(stats);

    pthread_mutex_unlock(&g_bufferMutex);

    sem_post(&g_filledSpaceSemaphore);
  }
}