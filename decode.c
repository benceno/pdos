#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf(1, "There is no inputing to be decoded!\n");
        exit();
    }
    
    unlink("result.txt");
    int fd = open("result.txt", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf(2, "decode: cannot create result.txt\n");
        exit();
    }

    int key = 14;

    for (int i = 1; argv[i] != 0; i++) {
        char *input = argv[i];
        char c;
        while ((c = *input) != 0) {
            if (c >= 'a' && c <= 'z') {
                c = (c - 'a' + key + 26) % 26 + 'a';
            } else if (c >= 'A' && c <= 'Z') {
                c = (c - 'A' + key + 26) % 26 + 'A';
            }
            *input = c;
            input++;
        }

        printf(fd, "%s ", argv[i]);
    }

    write(fd, "\n", 1);
    close(fd);

    exit();
}
