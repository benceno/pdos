// history.h
#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 10
#define MAX_CMD_LENGTH 128

// Declare the history array and related variables/functions
char history[HISTORY_SIZE][MAX_CMD_LENGTH];
int history_index = -1;  // Initially, no history command has been selected
int history_count = 0; 

void my_strncpy(char *dest, const char *src, int n) {
    while (n > 0 && *src != '\0') {
        *dest++ = *src++;
        n--;
    }
    // Null-terminate the destination string
    while (n > 0) {
        *dest++ = '\0';  // Fill the remaining space with null characters
        n--;
    }
}

// Function to add a command to history
int add_to_history(char *cmd) {
    if (cmd[0] == '\0') {
        return -1;  // Skip empty commands
    }
    my_strncpy(history[history_count % HISTORY_SIZE], cmd, MAX_CMD_LENGTH);
    history_count++;
    history_index = history_count; // Reset index for the next command input
    return history_index;
}
#endif