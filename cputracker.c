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
    return -1;
  }
  return 0;
}

int get_semaphore_value(sem_t *sem) {
  int sval;
  sem_getvalue(sem, &sval);
  return sval;
}

struct proc_stat *get_item_from_buffer() {
  int index = get_semaphore_value(&g_filledSpaceSemaphore);
  return g_buffer[index];
}

int main() {
  if (get_nproc(&g_nproc) == -1) {
    exit(EXIT_FAILURE);
  }
  g_nproc++;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    g_buffer[i] = malloc(g_nproc * sizeof(struct proc_stat));
    if (g_buffer[i] == NULL) {
      exit(EXIT_FAILURE);
    }
  }

  if (sem_init(&g_filledSpaceSemaphore, 0, 0) ||
      sem_init(&g_leftSpaceSemaphore, 0, BUFFER_SIZE)) {
    exit(EXIT_FAILURE);
  }

  pthread_t readerThreadId;
  pthread_t analyzerThreadId;
  pthread_create(&readerThreadId, NULL, reader, NULL);
  pthread_create(&analyzerThreadId, NULL, analyzer, NULL);
  pthread_join(readerThreadId, NULL);
  pthread_join(analyzerThreadId, NULL);

  pthread_mutex_destroy(&g_bufferMutex);
  sem_destroy(&g_filledSpaceSemaphore);
  sem_destroy(&g_leftSpaceSemaphore);
  for (int i = 0; i < BUFFER_SIZE; i++) {
    free(g_buffer[i]);
  }
  return 0;
}