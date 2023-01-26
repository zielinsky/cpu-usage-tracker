
struct proc_stat {
    char name[256];
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
};