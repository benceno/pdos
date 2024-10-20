#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int str_len(char* str) {
    char* start = str;
    while (*str != '\0') {
        str++;
    }
    return str - start; 
}

char* decode(char* str) {
    int key = 14;
    char* ptr = str;

    while (*ptr != '\0') {
        if (*ptr >= 'a' && *ptr <= 'z') {
            *ptr = 'a' + (*ptr - 'a' - key + 26) % 26;
        } else if (*ptr >= 'A' && *ptr <= 'Z') {
            *ptr = 'A' + (*ptr - 'A' - key + 26) % 26;
        }
        ptr++;
    }
    return str; 
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf(1, "No input to decode!\n");
        exit();
    }

    int fd = open("result.txt", O_WRONLY | O_CREATE);
    if (fd < 0) {
        printf(2, "Cannot open or create result.txt\n");
        exit();
    }


    for (int i = 1; i < argc; i++) {
        char* decoded_str = decode(argv[i]);
        int len = str_len(decoded_str);
        
        if (write(fd, decoded_str, len) != len) {
            printf(2, "Cannot write to result.txt\n");
            close(fd);
            exit();
        }

        if (i < argc - 1) {
            write(fd, " ", 1); 
        } else {
            write(fd, "\n", 1); 
        }
    }

    close(fd);
    exit();
}
