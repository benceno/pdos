#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{    
    // Try to open file
    int fd = open("nonexistent.txt", O_RDONLY);
    if(fd < 0) {
        exit();  // will crash here
    }
    
    printf(1, "Program finished successfully!\n");  // Should never reach this
    exit();
}