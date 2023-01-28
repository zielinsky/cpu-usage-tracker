#include "cputracker.h"
#include "analyzer.h"
#include "reader.h"

int g_nproc = 0;
struct proc_stat *g_buffer[BUFFER_SIZE];
pthread_mutex_t g_bufferMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t g_filledSpaceSemaphore;
sem_t g_leftSpaceSemaphore;

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
  int index = get_semaphore_value(&g_filledSpaceSemaphore);
  if (index > BUFFER_SIZE) {
    return -1;
  }
  g_buffer[index] = stats;
  return 0;
}

struct proc_stat *remove_item() {
  int indexToRemove = get_semaphore_value(&g_filledSpaceSemaphore);
  if (indexToRemove < 0) {
    return NULL;
  }
  struct proc_stat *stats = g_buffer[indexToRemove];
  return stats;
}

int main() {
  if (get_nproc(&g_nproc) == -1) {
    exit(EXIT_FAILURE);
  }

  if (sem_init(&g_filledSpaceSemaphore, 0, 0) ||
      sem_init(&g_leftSpaceSemaphore, 0, BUFFER_SIZE)) {
    exit(EXIT_FAILURE);
  }

  g_nproc++;
  pthread_t readerThreadId;
  pthread_t analyzerThreadId;
  pthread_create(&readerThreadId, NULL, reader, NULL);
  pthread_create(&analyzerThreadId, NULL, analyzer, NULL);
  pthread_join(readerThreadId, NULL);
  pthread_join(analyzerThreadId, NULL);
  return 0;
}