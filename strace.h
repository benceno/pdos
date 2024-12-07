#ifndef _STRACE_H_
#define _STRACE_H_

#define MAX_TRACE_EVENTS 500 
#define MAX_MESSAGE_LENGTH 150

struct trace_event {
  int pid;
  char command_name[16]; 
  char syscall_name[20];  // Enough space for syscall names
  int return_value;
  int valid; 
  
};

struct trace_flags {
    char syscall_filter[32];  // syscall filter
    int one_time_filter;      // one-time filter flag
    int success_only;         // -s flag
    int fail_only;           // -f flag
    int output_fd;
};

extern struct trace_flags trace_flags;

void            init_trace_buffer(void);
void            add_trace_event(int pid, const char *cmd_name, const char *sys_name, int ret_val);

#endif

