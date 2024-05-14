#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf(1, "Usage: sanity <number of processes>\n");
        exit();
    }
    int numberProcesses = atoi(argv[1]);
    int amount = numberProcesses*3;


    for(int i = 0; i < amount; i++){
        int pid = fork();
        if(pid<0){
            printf(1, "Fork failed\n");
            exit();
        }
    }
    int pid = 0;
    while(pid < amount){
        int *retime = malloc(sizeof(int *)), *rutime = malloc(sizeof(int *)), *stime = malloc(sizeof(int *));
        pid = wait2(retime, rutime, stime);
        pid++;
        // pid = wait2(&retime, &rutime, &stime);
    }
    return 0;
}