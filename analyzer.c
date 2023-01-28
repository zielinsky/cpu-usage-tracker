#include "analyzer.h"
#include "cputracker.h"


unsigned long average_cpu_usage(struct proc_stat prev, struct proc_stat next){
  unsigned long prev_idle = prev.idle + prev.iowait;
  unsigned long idle = next.idle + next.iowait;

  unsigned long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
  unsigned long non_idle = next.user + next.nice + next.system + next.irq + next.softirq + next.steal;

  unsigned long prev_total = prev_idle + prev_non_idle;
  unsigned long total = idle + non_idle;

  total -= prev_total;
  idle -= prev_idle;

  return (total-idle)*100/total;  
}


void *analyzer() {
  struct proc_stat *prev = malloc(g_nproc*sizeof(struct proc_stat));
  struct proc_stat *stats = NULL;
  while (1) {
    sem_wait(&g_filledSpaceSemaphore);

    pthread_mutex_lock(&g_bufferMutex);

    stats = get_item_from_buffer();
    for(int i = 0; i < g_nproc; i++){
      printf("%lu ", average_cpu_usage(prev[i], stats[i]));
      prev[i] = stats[i];
    }

    printf("\n");
    pthread_mutex_unlock(&g_bufferMutex);

    sem_post(&g_leftSpaceSemaphore);
  }
}