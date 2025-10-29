#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Número de procesos hijos a crear
#define N 10

// Función simple de "trabajo" (busy-wait)
// Consume tiempo de CPU para que el scheduler tenga que actuar.
void work()
{
  long i;
  // Aumentamos el trabajo 10 veces (corregido el límite para evitar overflow)
  for (i = 0; i < 2000000000; i++) { 
     asm volatile ("nop");
  }
}

int
main(void)
{
  int pids[N];
  int i;

  printf("Iniciando prueba de Lottery Scheduler con %d procesos...\n", N);

  // Crear N procesos 
  for (i = 0; i < N; i++) {
    pids[i] = fork();
    
    if (pids[i] < 0) {
      printf("fork falló\n");
      exit(1);
    }

    if (pids[i] == 0) {
      // Código del Proceso Hijo 
      
      // Asignar tickets distintos 
      int tickets = 50 * (i + 1);
      
      // Llamada a la nueva system call
      if (settickets(tickets) < 0) {
        printf("settickets falló para el hijo %d\n", i);
        exit(1);
      }
      
      printf("[Hijo %d, PID %d] Iniciado con %d tickets.\n", i, getpid(), tickets);
      
      // Realizar trabajo para consumir CPU
      work();
      
      // Terminar el proceso hijo
      printf("[Hijo %d, PID %d] Trabajo terminado.\n", i, getpid());
      exit(0);
    }
    // fin del Código del Hijo
  }

  // Código del Proceso Padre
  printf("Padre [PID %d] esperando a los %d hijos...\n", getpid(), N);
  
  // Esperar a que todos los hijos terminen
  for (i = 0; i < N; i++) {
    wait(0);
  }

  printf("Padre [PID %d] Todos los hijos han terminado. Prueba finalizada.\n", getpid());
  exit(0);
}