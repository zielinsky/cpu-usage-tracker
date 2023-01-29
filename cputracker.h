#ifndef CPUTRACKER_H
#define CPUTRACKER_H

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 5

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define TIMEOUT 2
#define THREADS 3

enum State {Reader = 0, Printer = 1, Analyzer = 2};

struct proc_stat {
  char name[256];
  unsigned long user, // Time spent with normal processing in user mode.
      nice,           // Time spent with niced processes in user mode.
      system,         // Time spent running in kernel mode.
      idle,           // Time spent in vacations twiddling thumbs.
      iowait,     // Time spent waiting for I/O to completed. This is considered
                  // idle time too.
      irq,        // Time spent serving hardware interrupts.
      softirq,    // Time spent serving software interrupts.
      steal,      // Time stolen by other operating systems running in a virtual
                  // environment.
      guest,      // Time spent for running a virtual CPU or guest OS under the
                  // control of the kernel.
      guest_nice; // Time spent running a niced guest (virtual CPU for guest
                  // operating systems under the control of the Linux kernel).
};


extern int g_nproc;
extern pthread_mutex_t g_dataBufferMutex;
extern sem_t g_dataFilledSpaceSemaphore;
extern sem_t g_dataLeftSpaceSemaphore;

extern pthread_mutex_t g_printBufferMutex;
extern sem_t g_printFilledSpaceSemaphore;
extern sem_t g_printLeftSpaceSemaphore;

unsigned long *get_item_from_print_buffer(void);
struct proc_stat *get_item_from_data_buffer(void);
void notify_watchdog(int id);
void main_test(void);
#endif
