#include "analyzer.h"
#include "cputracker.h"

static unsigned long average_cpu_usage(struct proc_stat prev,
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

void *analyzer(void *arg) {
  struct proc_stat *prev = malloc((unsigned long)g_nproc * sizeof(struct proc_stat));
  unsigned long *avg = malloc((unsigned long)g_nproc * sizeof(unsigned long));
  unsigned long *bufforAvg;
  struct proc_stat *stats = NULL;
  pthread_cleanup_push(free, prev);
  pthread_cleanup_push(free, avg);
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

    bufforAvg = get_item_from_print_buffer();
    memcpy(bufforAvg, avg, (unsigned long)g_nproc * sizeof(unsigned long));

    pthread_mutex_unlock(&g_printBufferMutex);

    sem_post(&g_printFilledSpaceSemaphore);
  }
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  return arg;
}
