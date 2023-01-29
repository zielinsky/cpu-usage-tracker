/// @file analyzer.c
#include "analyzer.h"

 /**
 * @brief Calculates cpu usage.
 * @param prev previous proc_stat
 * @param current current proc_stat
 * @return Cpu usage in %
 */
static unsigned long cpu_usage(struct proc_stat prev,
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

 /**
 * @brief Analyzer function
 *
 * The Analyzer is responsible for taking data from the buffer and processing it into % CPU usage.
 *
 */
void *analyzer(void *arg) {
  struct proc_stat *prev;
  unsigned long *avg;
  unsigned long *bufforAvg;
  struct proc_stat *stats;

  (void)arg;

  if ((prev = malloc((unsigned long)g_nproc * sizeof(struct proc_stat))) ==
      NULL) {
    return NULL;
  }
  if ((avg = malloc((unsigned long)g_nproc * sizeof(unsigned long))) == NULL) {
    free(prev);
    return NULL;
  }
  pthread_cleanup_push(free, prev) pthread_cleanup_push(free, avg) while (1) {
    notify_watchdog(Analyzer);

    sem_wait(&g_dataFilledSpaceSemaphore);
    pthread_mutex_lock(&g_dataBufferMutex);

    stats = get_item_from_data_buffer();
    for (int i = 0; i < g_nproc; i++) {
      avg[i] = cpu_usage(prev[i], stats[i]);
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
}
