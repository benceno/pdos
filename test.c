#include "types.h"
#include "stat.h"
#include "user.h"
#include "rand.c"

#define PROC_NUM 5

void exit_child(int pid) {
  // int p;

  // p = wait();
  // printf(1, "process %d exited\n", p);
  wait();
}


int main(int argc, char **argv)
{
  int pid[PROC_NUM]; //여러 개 process 담을 배열
  
  ticketset(100);

  printf(1, "Test Case 1 : every ticket is 10\n");
  for (int i = 0; i < PROC_NUM; i++) {
    pid[i] = fork();
    if (pid[i] == 0) {
      //child process
      printf(1, "process %d my ticket %d\n", getpid(), ticketget());
      exit();
    }
  }
  for(int i = 0; i < PROC_NUM; i++)
    exit_child(pid[i]);


  ////////////////////////
  printf(1, "\n\nTest Case 2 : different number of ticket (50, 40, 30, 20, 10)\n");
  rand_index = 0;
  for (int i = 0; i < PROC_NUM; i++) {
    pid[i] = fork();
    if (pid[i] == 0) {
      //child process
      ticketset(50 - i*10);
      printf(1, "process %d my ticket %d\n", getpid(), ticketget());
      exit();
    }
  }
  for(int i = 0; i < PROC_NUM; i++)
    exit_child(pid[i]);

  ////////////////////////
  printf(1, "\n\nTest Case 3 : different number of ticket (30, 20, 10, 50, 40)\n");
  rand_index = 0;
  int new_rand_arr[SIZE] = {52, 40, 20, 10, 15};
  for (int i = 0; i < SIZE; i++) {
    rand_array[i] = new_rand_arr[i];
  }

  int ticket_array[PROC_NUM] = {30, 20, 10, 50, 40};

  for (int i = 0; i < PROC_NUM; i++) {
    pid[i] = fork();
    if (pid[i] == 0) {
      //child process
      ticketset(ticket_array[i]);
      printf(1, "process %d my ticket %d\n", getpid(), ticketget());
      exit();
    }
  }
  for(int i = 0; i < PROC_NUM; i++)
    exit_child(pid[i]);

  exit();
}