#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int me = getpid();
  int padre = getppid();
  printf("Soy PID=%d, mi padre es PID=%d\n", me, padre);
  exit(0);
}
