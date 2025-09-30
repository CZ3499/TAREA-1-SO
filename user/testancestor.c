#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid_hijo, pid_nieto;

  printf("--- Test de getancestor ---\n");
  printf("Soy el proceso original (abuelo), mi PID es %d\n", getpid());

  pid_hijo = fork();

  if (pid_hijo == 0) {
    printf("Soy el proceso hijo, mi PID es %d. Mi padre es %d\n", getpid(), getppid());

    pid_nieto = fork();
    if (pid_nieto == 0) {
      int mi_pid = getpid();
      int padre_pid = getppid();
      int abuelo_pid = getancestor(2);

      printf("Soy el proceso nieto, mi PID es %d. Mi padre es %d\n", mi_pid, padre_pid);
      printf("Resultados de getancestor desde el nieto:\n");
      printf(" -> getancestor(0) [yo mismo]: %d\n", getancestor(0));
      printf(" -> getancestor(1) [mi padre]: %d\n", getancestor(1));
      printf(" -> getancestor(2) [mi abuelo]: %d\n", abuelo_pid);
      printf(" -> getancestor(3) [bisabuelo/shell]: %d\n", getancestor(3));
      printf(" -> getancestor(10) [inválido]: %d\n", getancestor(10)); // Probando un caso inválido
    } else {
      wait(0);
    }
  } else {
    wait(0);
  }

  exit(0);
}