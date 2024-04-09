#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

static int count_numof_digits(int n) {
    int counter = 0;
    do {
    n /= 10;
    ++counter;
  } while (n != 0);
  return counter;
}

int sys_count_num_of_digits(void) {
    return count_numof_digits(myproc()->tf->ebx);
}