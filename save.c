#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void
save(int fd, int argc, char *argv[])
{
  int i;

  for(i = 2; i < argc; i++)
  {
    write(fd, argv[i], strlen(argv[i]));
    printf(1, "%s", argv[i]);
  }

}

int main(int argc, char *argv[])
{
  int fd;

  if(argc <= 1)
  {
    printf(1, "save: need 2 arguments\n");
    exit();
  }

  if((fd = open(argv[1], O_RDWR | O_CREATE)) < 0)
  {
    printf(1, "save: cannot open %s\n", argv[1]);
    exit();
  }
  else
  {
    save(fd, argc, argv);
    close(fd);
  }

  exit();
}
