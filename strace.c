#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    if(argc < 2) {
        printf(1, "Usage: strace on|off|run|dump|-e|-s|-f [args]\n");
        exit();
    }

    if(strcmp(argv[1], "-e") == 0) {
        if(argc < 3) {
            printf(1, "Usage: strace -e <syscall>\n");
            exit();
        }
        strace(1);             
        strace_filter(argv[2]);
    }
    else if(strcmp(argv[1], "-s") == 0) {
        if(argc > 2 && strcmp(argv[2], "-e") == 0) {
            if(argc < 4) {
                printf(1, "Usage: strace -s -e <syscall>\n");
                exit();
            }
            strace(1);
            strace_filter(argv[3]);
            strace_success(1);
        } else {
            strace(1);
            strace_success(1);
        }
    }
    else if(strcmp(argv[1], "-f") == 0) {
        if(argc > 2 && strcmp(argv[2], "-e") == 0) {
            if(argc < 4) {
                printf(1, "Usage: strace -f -e <syscall>\n");
                exit();
            }
            strace(1);
            strace_filter(argv[3]);
            strace_fail(1);
        } else {
            strace(1);
            strace_fail(1);
        }
    }
    else if(strcmp(argv[1], "-o") == 0) {
        if(argc < 4) {
            printf(1, "Usage: strace -o <filename> <command>\n");
            exit();
        }
        int fd = open(argv[2], O_CREATE | O_WRONLY);
        if(fd < 0) {
            printf(1, "Cannot create file %s\n", argv[2]);
            exit();
        }
        
        
        close(1); 
        dup(fd);  
        close(fd);
        
        strace(1);
        int pid = fork();
        if(pid < 0) {
            printf(2, "fork failed\n");
            exit();
        }
        if(pid == 0) {
            exec(argv[3], argv + 3);
            printf(2, "exec %s failed\n", argv[3]);
            exit();
        }
        wait();
        strace(0);
    }
    else if(strcmp(argv[1], "on") == 0) {
        strace(1);
    }
    else if(strcmp(argv[1], "off") == 0) {
        strace(0);
    }
    else if(strcmp(argv[1], "run") == 0) {
        if(argc < 3) {
            printf(1, "Usage: strace run <command> [args...]\n");
            exit();
        }
        int pid = fork();
        if(pid < 0) {
            printf(1, "fork failed\n");
            exit();
        }
        if(pid == 0) {
            strace(1); 
            exec(argv[2], argv + 2);
            printf(1, "exec %s failed\n", argv[2]);
            exit();
        } else {
            wait();
            strace(0);
        }
    }
    else if(strcmp(argv[1], "dump") == 0) {
        stracedump();
    }
    else {
        printf(1, "Invalid argument: use 'on', 'off', 'run', 'dump', '-e', '-s', or '-f'\n");
    }
    exit();
}