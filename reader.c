#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reader.h"

int main() {
    FILE* file = fopen("/proc/stat", "r");
    char line[1024];
    struct proc_stat* stats = NULL;
    int thread_count = 0;
    int capacity = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "cpu", 3) == 0) {
            if (thread_count == capacity) {
                capacity = capacity == 0 ? 1 : capacity * 2;
                struct proc_stat* temp = realloc(stats, capacity * sizeof(struct proc_stat));
                if(temp == NULL) {
                    perror("Error reallocating memory");
                    free(stats);
                    fclose(file);
                    return 1;
                }
                stats = temp;
            }
            sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", stats[thread_count].name, &stats[thread_count].user, &stats[thread_count].nice, &stats[thread_count].system, &stats[thread_count].idle, &stats[thread_count].iowait, &stats[thread_count].irq, &stats[thread_count].softirq, &stats[thread_count].steal, &stats[thread_count].guest, &stats[thread_count].guest_nice);
            thread_count++;
        }else{
            break;
        }
    }

    for (int i = 0; i < thread_count; i++) {
        printf("%s %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", stats[i].name, stats[i].user, stats[i].nice, stats[i].system, stats[i].idle, stats[i].iowait, stats[i].irq, stats[i].softirq, stats[i].steal, stats[i].guest, stats[i].guest_nice);
    }

    free(stats);
    fclose(file);
    return 0;
}