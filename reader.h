#ifndef READER_H
#define READER_H
#include "cputracker.h"

#define READ_DELAY 50000

int get_proc_stats(struct proc_stat *stats);
void *reader();

#endif