#include "cputracker.h"
#include "reader.h"
#include <stdio.h>
#include <stdlib.h>

int nproc = 0;
struct proc_stat *BUFFER[BUFFER_SIZE];
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t filledSpaceSemaphore;
sem_t leftSpaceSemaphore;

int get_nproc(int *nproc) {
  *nproc = sysconf(_SC_NPROCESSORS_ONLN);
  if (*nproc == -1) {
    perror("Reading number of threads failed");
    return -1;
  }
  return 0;
}

int get_semaphore_value(sem_t *sem) {
  int sval;
  sem_getvalue(sem, &sval);
  return sval;
}

int put_item(struct proc_stat *stats) {
  int index = get_semaphore_value(&filledSpaceSemaphore);
  if (index > BUFFER_SIZE) {
    return -1;
  }
  BUFFER[index] = stats;
  return 0;
}

int main() {
  if (get_nproc(&nproc) == -1) {
    exit(EXIT_FAILURE);
  }

  if (sem_init(&filledSpaceSemaphore, 0, 0) ||
      sem_init(&leftSpaceSemaphore, 0, BUFFER_SIZE)) {
    exit(EXIT_FAILURE);
  }

  nproc++;
  reader();
  return 0;
}