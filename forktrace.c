#include "types.h"
#include "user.h"

void child_work(int id, int level) {
    printf(1, "Child %d at level %d starting work\n", id, level);
    
    sleep(10);  
    getpid(); 
    
    printf(1, "Child %d at level %d finishing work\n", id, level);
}

int
main(void)
{
    printf(1, "Fork trace test starting\n");
    
    // Create first child
    int pid1 = fork();
    if(pid1 < 0) {
        printf(2, "fork failed\n");
        exit();
    }
    
    if(pid1 == 0) {
        // First child
        child_work(1, 1);
        
        // First child creates another child
        int pid2 = fork();
        if(pid2 < 0) {
            printf(2, "fork failed\n");
            exit();
        }
        
        if(pid2 == 0) {
            // Second child
            child_work(2, 2);
            exit();
        }
        
        // First child waits for its child
        wait();
        exit();
    }
    
    // Parent creates another child
    int pid3 = fork();
    if(pid3 < 0) {
        printf(2, "fork failed\n");
        exit();
    }
    
    if(pid3 == 0) {
        // Third child
        child_work(3, 1);
        exit();
    }
    
    // Parent waits for all children
    wait();
    wait();
    
    printf(1, "Fork trace test finished\n");
    exit();
}