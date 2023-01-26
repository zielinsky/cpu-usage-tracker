#ifndef READER_H
#define READER_H
#include <unistd.h>

int print_stats();
struct proc_stat *get_proc_stats();
int get_nproc();

#endif