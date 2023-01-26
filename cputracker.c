#include "cputracker.h"
#include "reader.h"

int nproc = 0;

int main(){
    nproc = get_nproc() + 1;

    print_stats();
    return 0;
}