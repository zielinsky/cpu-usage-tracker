#include "reader.h"
#include "cputracker.h"

static int get_proc_stats(struct proc_stat *stats) {
  FILE *file = fopen("/proc/stat", "r");
  char line[1024];
  for (int thread = 0; thread < g_nproc; thread++) {
    if (fgets(line, sizeof(line), file) == NULL ||
        strncmp(line, "cpu", 3) != 0) {
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

void *reader(void *arg) {
  (void)arg;
  while (1) {
    notify_watchdog(Reader);
    sem_wait(&g_dataLeftSpaceSemaphore);
    pthread_mutex_lock(&g_dataBufferMutex);

    if (get_proc_stats(get_item_from_data_buffer()) == -1) {
      pthread_mutex_unlock(&g_dataBufferMutex);
      sem_post(&g_dataLeftSpaceSemaphore);
      continue;
    }
    
    pthread_mutex_unlock(&g_dataBufferMutex);
    sem_post(&g_dataFilledSpaceSemaphore);
    usleep(READ_DELAY);
  }
}
