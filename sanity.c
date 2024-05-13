#include "types.h"
#include "user.h"
#define CPU_BOUND 0
#define S_BOUND 1
#define IO_BOUND 2

void CPU_Bound (void) {
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 1000000; j++){}
  }
}

void S_Bound (void) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 1000000; j++){}
    yield();
  }
}

void IO_Bound (void) {
  for (int i = 0; i < 100; i++) {
    sleep(1);
  }
}

void Create_Process (int type) {
  switch (type) {
    case CPU_BOUND:
      CPU_Bound ();
      break;
    
    case S_BOUND:
      S_Bound ();
      break;

    case IO_BOUND:
      IO_Bound ();
      break;
  }
}

int main (int argc, char *argv[]) {

  if (argc != 2) {
    printf (1, "One parameter expected\n");
    exit();
  }

  int num_processes = atoi(argv[1]) * 3; //quantidade de processos
  int processes_amount[3] = {0,0,0};
  int sum_ready_time[3] = {0,0,0};
  int sum_sleep_time[3] = {0,0,0};
  int sum_turnaround_time[3] = {0,0,0};

  for (int i = 0; i < num_processes; i++) {
    int process_id = fork();
    if (process_id == 0) { //processo filho
      int type = getpid() % 3;
      Create_Process (type); //chama funcao para executar a atividade
      exit();
    }
    else { //processo pai
      //atualiza contador de processos para o tipo de atividade do processo filho
      processes_amount[process_id % 3]++;
    }
  }
  
  // Chamar função wait2 e calcular rutime, retime e stime médios
  for (int i = 0; i < num_processes; i++){
    int retime, rutime, stime;
    int childPid = wait2 (&retime, &rutime, &stime);
    char *type;
    if ( childPid % 3 == 0){
      type = "CPU_BOUND";
    } else if (childPid % 3 == 1){
      type = "S_BOUND";
    } else{
      type = "IO_BOUND";
    }

    printf (1, "PID: %d\n Type: %s\n Ready time: %d\n Run time: %s\n Sleep time: %d\n\n", childPid, type, retime, rutime, stime);
    sum_ready_time[childPid % 3] += retime;
    sum_sleep_time[childPid % 3] += stime;
    sum_turnaround_time[childPid % 3] += retime + rutime + stime;
  }

  //calcula e imprime as medias
  for (int j = 0; j < 3; j++) {
    char *type;
    if (j == 0) {
      type = "CPU_BOUND";
    }
    else if (j == 1) {
      type = "S_BOUND";
    }
    else {
      type = "IO_BOUND";
    }

    printf (1, "%s Contador de processos: %d\n", type, processes_amount[j]);
    printf (1, "%s Média ready time: %d\n", type, sum_ready_time[j] / processes_amount[j]);
    printf (1, "%s Média sleeping time: %d\n", type, sum_sleep_time[j] / processes_amount[j]);
    printf (1, "%s Média turnaround time: %d\n", type, sum_turnaround_time[j] / processes_amount[j]);

   
  }

  exit();

}