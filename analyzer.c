#include "analyzer.h"
#include "cputracker.h"

unsigned long average_cpu_usage(struct proc_stat prev,
                                struct proc_stat current) {
  unsigned long prevIdle = prev.idle + prev.iowait;
  unsigned long idle = current.idle + current.iowait;

  unsigned long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq +
                              prev.softirq + prev.steal;
  unsigned long nonIdle = current.user + current.nice + current.system +
                          current.irq + current.softirq + current.steal;

  unsigned long prevTotal = prevIdle + prevNonIdle;
  unsigned long total = idle + nonIdle;

  total -= prevTotal;
  idle -= prevIdle;

  return (total - idle) * 100 / total;
}

void *analyzer() {
  struct proc_stat *prev = malloc(g_nproc * sizeof(struct proc_stat));
  unsigned long *avg = malloc(g_nproc * sizeof(unsigned long));
  struct proc_stat *stats = NULL;
  while (1) {
    sem_wait(&g_dataFilledSpaceSemaphore);

    pthread_mutex_lock(&g_dataBufferMutex);

    stats = get_item_from_data_buffer();
    for (int i = 0; i < g_nproc; i++) {
      avg[i] = average_cpu_usage(prev[i], stats[i]);
      prev[i] = stats[i];
    }

    pthread_mutex_unlock(&g_dataBufferMutex);

    sem_post(&g_dataLeftSpaceSemaphore);

    //

    sem_wait(&g_printLeftSpaceSemaphore);

    pthread_mutex_lock(&g_printBufferMutex);

    unsigned long *bufforAvg = get_item_from_print_buffer();
    memcpy(bufforAvg, avg, g_nproc * sizeof(unsigned long));

    pthread_mutex_unlock(&g_printBufferMutex);

    sem_post(&g_printFilledSpaceSemaphore);
  }
  free(prev);
  free(avg);
}