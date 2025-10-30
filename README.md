# TAREA 2 Sistemas operativos Sec 2

## **Integrantes**

- **Martin Peralta** <br>
  <martperalta@alumnos.uai.cl>

- **Cristobal Zeballos** <br>
  <czeballos@alumnos.uai.cl>

## Descripción 
El objetivo de esta tarea fue implementar un planificador de procesos basado en Lottery Scheduling en xv6, reemplazando la política round-robin tradicional por un mecanismo probabilístico. Esto permite que la fracción de uso del CPU que recibe em cada proceso sea proporcional al número de tickets que se asignan.
Para lograrlo esto se creó la llamada al sistema **settickets(int n)**, que permite a cada proceso definir cuántos tickets tendrá, garantizando siempre un mínimo de 1. Además, se agregaron dos variables clave en la estructura del proceso: **tickets**, que almacena la cantidad de boletos asignados, y **cpu_slices**, que registra cuántas veces un proceso fue elegido por el planificador.
En la función **scheduler()**, el mecanismo de selección se reemplazó por un sorteo con la siguiente lógica:
1) Se suma el total de tickets de los procesos en estado RUNNABLE.

2) Se genera un número aleatorio entre 1 y ese total.

3) Se recorre la lista de procesos, acumulando sus tickets hasta que el acumulado supere el número sorteado; en ese momento se elige el proceso ganador.

4) Antes de ejecutarlo, se incrementa el contador **cpu_slices** para registrar su uso de el CPU.
---
## Archivos Modificados

### Parte I: Implementación de `settickets(int)`
- `kernel/syscall.h`: se agregó la constante `SYS_settickets`.
- `kernel/syscall.c`: se añadió la entrada en la tabla de llamadas para `sys_settickets`.
- `kernel/sysproc.c`: se implementó la función `sys_settickets`, que recibe un entero n que actualiza el campo `tickets` del proceso actual (forzando un mínimo de 1).
- `user/user.h`: se declaró el prototipo `int settickets(int);`.
- `user/usys.S`: se añadió la entrada `SYSCALL(settickets)` para generar el wrapper accesible desde el usuario.

### Parte II: Modificación del planificador
- `kernel/proc.h`: se agregaron los nuevos campos a la estructura del proceso:
       - `int tickets`; (inicializado en 100).
       - `int cpu_slices`; (contador de veces que fue planificado).

- `kernel/proc.c`:
       - En `allocproc()` y `userinit()` se inicializaron los valores por defecto de `tickets` y `cpu_slices`.
       - En `scheduler()` se reemplazó la política round-robin por la lógica de Lottery Scheduling:

            - Sumar tickets del proceso RUNNABLE.
            - Generar un número aleatorio en [1, total].
            - Recorrer el proceso acumulando de tickets hasta encontrar el ganador.
            - Incrementar `cpu_slices` antes de ser ejecutado.

### Parte III: Programas de prueba
- `user/tickets.c`: programa simple para modificar desde la shell la cantidad de tickets de un proceso y verificar la syscall.

- `user/demo.c`: programa que crea múltiples procesos hijos con diferentes tickets, ejecuta trabajo de el CPU y muestra resultados, permitiendo observar la proporcionalidad del Lottery Scheduling.

- `Makefile`: se añadieron los nuevos programas `tickets` y `demo` a la lista `UPROGS` para que se compilen y estén disponibles en la shell del xv6.
---
## Pruebas y Dificultades

Durante la implementación de la parte del planificador en `scheduler()`, se presentó un error común al no colocar exactamente la variable `total` la cual acumula los tickets de los procesos en el estado RUNNABLE. El valor quedaba indefinido en algunas ejecuciones, provocando comportamientos inesperados como que ningún proceso fuera seleccionado o que el sistema entrara en un ciclo infinito sin respuesta.

El fallo fue identificado al ver que con varios procesos en ejecución el planificador imprimía siempre `total=0`. La corrección consistió en colocar explícitamente `total = 0;` al inicio de cada iteración del `scheduler()`.
---
## Conclusiones

Como conclusión de esta tarea de implementación de la llamada al sistema `settickets` el cual nos permitió introducir los parámetros dinámicos para los procesos, mientras que la modificación del planificador `scheduler()` representó un desafío mayor, al reemplazar el esquema round-robin por un mecanismo probabilístico basado en sorteo debido a que se debía entender como funcionaba para asi realizar el cambio de manera efectica.

En el camino, pudimos reforzar la importancia de coordinar los cambios en la lógica del kernel y en la del interfaz de usuario, así entendiendo cómo las llamadas al sistema se convierten en el puente que conecta la gestión interna de los procesos con los programas que los utilizan. Asimismo, las variables agregadas `tickets` y `cpu_slices` nos permitieron experimentar directamente con la relación entre asignación de el CPU y los parámetros definidos por el usuario, comprobando que el nuevo planificador distribuye el tiempo de el CPU de forma proporcional los tickets asignados.

Esta tarea nos permitió profundizar en el rol del planificador de procesos y en el funcionamiento de las syscalls en xv6, consolidando lo aprendido en la tarea anterior y enfrentando un nivel mayor de complejidad, al tener que diseñar, implementar y validar un mecanismo de planificación comprobable dentro del sistema operativo.
---
