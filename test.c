#include "types.h"
#include "stat.h"
#include "user.h"
#include "rand.c"

void exit_child(int pid) {
  // int p;

  // p = wait();
  // printf(1, "process %d exited\n", p);
  wait();
}


int main(int argc, char **argv)
{
  //int p;
  int pid[10]; //여러 개 process

  printf(1, "test start\n");

  //rand test
  for (int i = 0; i < 20; i++) {
    //printf(1, "rand:%d\n", random());
  }
  /////////

  //printf(1, "process %d my ticket %d [parent]\n", getpid(), ticketget());

  for (int i = 0; i < 10; i++) {
    pid[i] = fork();
    if (pid[i] == 0) {
      //child process
      ticketset(15 - i);
      printf(1, "process %d my ticket %d\n", getpid(), ticketget());
      exit();
    }
  }




  // p = fork();

  // if (p != 0) {
  //   printf(1, "process %d my ticket %d\n", getpid(), ticketget());
  // }
  // else if (p == 0) {
  //  // printf(1, "process %d my ticket %d\n", getpid(), ticketget());
  //   exit();
  // }


  for(int i = 0; i < 10; i++)
    exit_child(pid[i]);
  exit();
}