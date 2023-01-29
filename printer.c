#include "printer.h"

void *printer() {
  unsigned long *avg = NULL;
  while (1) {
    sem_wait(&g_printFilledSpaceSemaphore);

    pthread_mutex_lock(&g_printBufferMutex);

    system("clear");
    avg = get_item_from_print_buffer();
    for (int i = 0; i < g_nproc; i++) {
      printf("[%d]: \033[38;2;%lu;%lu;0m%lu\033[0m\n", i,avg[i]*5/2,255-avg[i]*5/2, avg[i]);
    }

    pthread_mutex_unlock(&g_printBufferMutex);

    sem_post(&g_printLeftSpaceSemaphore);
    sleep(1);
  }
}