#ifndef ANALYZER_H
#define ANALYZER_H
#include "cputracker.h"

void *analyzer();
unsigned long average_cpu_usage(struct proc_stat prev, struct proc_stat next);
#endif