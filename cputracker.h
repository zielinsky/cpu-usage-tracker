#ifndef CPUTRACKER_H
#define CPUTRACKER_H

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

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
extern struct proc_stat *g_buffer[];
extern pthread_mutex_t g_bufferMutex;
extern sem_t g_filledSpaceSemaphore;
extern sem_t g_leftSpaceSemaphore;

int get_nproc();
int get_semaphore_value(sem_t *sem);
int put_item(struct proc_stat *stats);
#endif