/// @file cputracker.c
#include "cputracker.h"
#include "analyzer.h"
#include "printer.h"
#include "reader.h"

int g_nproc = 0;
static struct proc_stat *g_dataBuffer[BUFFER_SIZE];
pthread_mutex_t g_dataBufferMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t g_dataFilledSpaceSemaphore;
sem_t g_dataLeftSpaceSemaphore;

static unsigned long *g_printBuffer[BUFFER_SIZE];
pthread_mutex_t g_printBufferMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t g_printFilledSpaceSemaphore;
sem_t g_printLeftSpaceSemaphore;

static pthread_t readerThreadId;
static pthread_t analyzerThreadId;
static pthread_t printerThreadId;

static int working[THREADS];
static pthread_mutex_t watchdogMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Signal handler
 * @param signum signal number
 */
static void handler(int signum) {
  (void)signum;
  pthread_cancel(readerThreadId);
  pthread_cancel(printerThreadId);
  pthread_cancel(analyzerThreadId);
}

/**
 * @brief Function retrieving the number of threads
 * 
 * @param signum signal number
 *
 * @return 0 if the number of threads has been retrieved else -1
 */
static int get_nproc(int *nproc) {
  *nproc = (int)sysconf(_SC_NPROCESSORS_ONLN);
  if (*nproc == -1) {
    return -1;
  }
  return 0;
}

/**
 * @brief Function getting the semaphore value
 * 
 * @param sem semaphore
 *
 * @return sem value
 */
static int get_semaphore_value(sem_t *sem) {
  int sval;
  sem_getvalue(sem, &sval);
  return sval;
}

/**
 * @brief Function getting item in data buffer
 * 
 * @return proc_stat struct from data buffer
 */
struct proc_stat *get_item_from_data_buffer(void) {
  int index = get_semaphore_value(&g_dataFilledSpaceSemaphore);
  return g_dataBuffer[index];
}

/**
 * @brief Function getting item in print buffer
 * 
 * @return proc_stat struct from print buffer
 */
unsigned long *get_item_from_print_buffer(void) {
  int index = get_semaphore_value(&g_printFilledSpaceSemaphore);
  return g_printBuffer[index];
}

/**
 * @brief Notify the watchdog that thread is alive
 * 
 * @param id Thread ID
 */
void notify_watchdog(int id) {
  pthread_mutex_lock(&watchdogMutex);
  working[id] = 1;
  pthread_mutex_unlock(&watchdogMutex);
}

/**
 * @brief Watchdog function.
 *
 * This function checks whether the threads are alive.
 * If a thread does not speak for 2 seconds the watchdog kills the program
 */
static void watchdog(void) {
  while (1) {
    sleep(TIMEOUT);
    pthread_mutex_lock(&watchdogMutex);
    for (int i = 0; i < THREADS; i++) {
      if (working[i] == 0) {
        pthread_mutex_unlock(&watchdogMutex);
        printf("Error: Thread not responding... Terminating\n");
        handler(-1);
        return;
      }
      working[i] = 0;
    }
    pthread_mutex_unlock(&watchdogMutex);
  }
}

/**
 * @brief Simple test function with asserts.
 */
void main_test(void) {
  int nproc;
  if (get_nproc(&nproc) == 0) {
    assert(nproc >= 1);
  }
}

int main(void) {
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));

  if (get_nproc(&g_nproc) == -1) {
    exit(EXIT_FAILURE);
  }
  g_nproc++;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    g_dataBuffer[i] = malloc((unsigned long)g_nproc * sizeof(struct proc_stat));
    g_printBuffer[i] = malloc((unsigned long)g_nproc * sizeof(unsigned long));
    if (g_dataBuffer[i] == NULL || g_printBuffer[i] == NULL) {
      exit(EXIT_FAILURE);
    }
  }

  if (sem_init(&g_dataFilledSpaceSemaphore, 0, 0) ||
      sem_init(&g_dataLeftSpaceSemaphore, 0, BUFFER_SIZE)) {
    exit(EXIT_FAILURE);
  }

  if (sem_init(&g_printFilledSpaceSemaphore, 0, 0) ||
      sem_init(&g_printLeftSpaceSemaphore, 0, BUFFER_SIZE)) {
    exit(EXIT_FAILURE);
  }

  pthread_create(&readerThreadId, NULL, reader, NULL);
  pthread_create(&analyzerThreadId, NULL, analyzer, NULL);
  pthread_create(&printerThreadId, NULL, printer, NULL);

  action.sa_handler = handler;
  sigaction(SIGTERM, &action, NULL);

  watchdog();

  pthread_join(readerThreadId, NULL);
  pthread_join(analyzerThreadId, NULL);
  pthread_join(printerThreadId, NULL);

  pthread_mutex_destroy(&g_dataBufferMutex);
  sem_destroy(&g_dataFilledSpaceSemaphore);
  sem_destroy(&g_dataLeftSpaceSemaphore);

  pthread_mutex_destroy(&g_printBufferMutex);
  sem_destroy(&g_printFilledSpaceSemaphore);
  sem_destroy(&g_printLeftSpaceSemaphore);

  pthread_mutex_destroy(&watchdogMutex);
  for (int i = 0; i < BUFFER_SIZE; i++) {
    free(g_dataBuffer[i]);
    free(g_printBuffer[i]);
  }
  return 0;
}
