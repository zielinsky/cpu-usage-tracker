#ifndef READER_H
#define READER_H
#include "cputracker.h"

int get_proc_stats(struct proc_stat *stats);
void *reader();

#endif