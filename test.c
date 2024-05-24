#include "types.h"
#include "stat.h"
#include "user.h"
#include "rand.c"

void exit_child(int pid) {
  int p = wait();
  printf(1, "process %d exited\n", p);
}


int main(int argc, char **argv)
{
  int p;

  printf(1, "test start\n");

  //rand test
  for (int i = 0; i < 20; i++) {
    printf(1, "rand:%d\n", random());
  }
  /////////
  p = fork();

  if (p != 0) {
    printf(1, "process %d my ticket %d\n", getpid(), ticketget());
  }
  else if (p == 0) {
   // printf(1, "process %d my ticket %d\n", getpid(), ticketget());
    exit();
  }

  exit_child(p);
  exit();
}