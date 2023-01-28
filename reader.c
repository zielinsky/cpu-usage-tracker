#include "reader.h"

int get_proc_stats(struct proc_stat *stats) {
  FILE *file = fopen("/proc/stat", "r");
  char line[1024];
  for (int thread = 0; thread < g_nproc; thread++) {
    fgets(line, sizeof(line), file);
    if (strncmp(line, "cpu", 3) != 0) {
      perror("Reading thread info failed");
      fclose(file);
      return -1;
    }

    sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           stats[thread].name, &stats[thread].user, &stats[thread].nice,
           &stats[thread].system, &stats[thread].idle, &stats[thread].iowait,
           &stats[thread].irq, &stats[thread].softirq, &stats[thread].steal,
           &stats[thread].guest, &stats[thread].guest_nice);
  }
  fclose(file);
  return 0;
}

void *reader() {
  struct proc_stat *stats = NULL;
  while (1) {

    sem_wait(&g_leftSpaceSemaphore);

    pthread_mutex_lock(&g_bufferMutex);

    stats = get_item_from_buffer();
    
    if (get_proc_stats(stats) == -1) {
      pthread_mutex_unlock(&g_bufferMutex);
      continue;
    }

    pthread_mutex_unlock(&g_bufferMutex);

    sem_post(&g_filledSpaceSemaphore);
    usleep(READ_DELAY);
  }
}