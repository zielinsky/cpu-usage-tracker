#include "analyzer.h"
#include "cputracker.h"
#include <stdio.h>
int x = 0;

void *analyzer() {
  while (1) {
    sem_wait(&g_filledSpaceSemaphore);
    pthread_mutex_lock(&g_bufferMutex);

    struct proc_stat *stats = remove_item();

    pthread_mutex_unlock(&g_bufferMutex);
    sem_post(&g_leftSpaceSemaphore);

    for (int i = 0; i < g_nproc; i++) {
      printf("%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", stats[i].name,
             stats[i].user, stats[i].nice, stats[i].system, stats[i].idle,
             stats[i].iowait, stats[i].irq, stats[i].softirq, stats[i].steal,
             stats[i].guest, stats[i].guest_nice);
    }
    free(stats);
  }
}