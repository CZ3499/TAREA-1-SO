# TAREA 2: Sistemas Operativos
## **Integrantes**

-   **Martín Peralta** <br>
    <martperalta@alumnos.uai.cl>

-   **Cristóbal Zeballos** <br>
    <czeballos@alumnos.uai.cl>

## Descripción

El objetivo de esta tarea fue reemplazar el planificador Round-Robin de xv6 por un sistema de **Lottery Scheduling** (planificación por lotería). Con este mecanismo, la fracción de CPU que recibe cada proceso es proporcional a la cantidad de "tickets" que posee.

Para lograr esto, se implementó la llamada al sistema `settickets(int n)`, que permite a un proceso modificar su propia cantidad de *tickets*, asegurando siempre un mínimo de 1.

Se modificó la estructura del proceso (`struct proc`) para incluir dos nuevos campos:
1.  `int tickets`: Almacena la cantidad de boletos del proceso, inicializado en 100.
2.  `int cpu_slices`: Un contador que registra cuántas veces el proceso ha sido elegido por el planificador.

Finalmente, la lógica de la función `scheduler()` fue reemplazada por un sorteo probabilístico:
1.  Se calcula el `total_tickets` sumando los *tickets* de todos los procesos en estado `RUNNABLE`.
2.  Se genera un número aleatorio ("ticket ganador") entre 1 y el `total_tickets`.
3.  Se itera sobre los procesos `RUNNABLE`, acumulando sus *tickets*. El primer proceso que logre que el acumulador sea mayor o igual al *ticket* ganador es seleccionado.
4.  Antes de ejecutar el proceso (con `swtch`), se incrementa su contador `cpu_slices`.

---

## Archivos Modificados

### Parte I: Implementación de la System Call `settickets(int)`

-   `kernel/syscall.h`: Se agregó la constante `SYS_settickets` (con valor 22).
-   `kernel/syscall.c`: Se añadió la función `sys_settickets` al arreglo `syscalls[]` para registrarla.
-   `kernel/sysproc.c`: Se implementó la lógica de `sys_settickets(void)`. Esta función obtiene el argumento entero `n`, valida que sea como mínimo 1, y lo asigna al campo `tickets` del proceso actual (`myproc()`).
-   `user/user.h`: Se declaró el prototipo `int settickets(int);` para el espacio de usuario.
-   `user/usys.pl`: Se añadió la entrada `entry("settickets");` para que el script Perl genere el *wrapper* de la *syscall* en `usys.S`.

### Parte II: Modificación del Kernel (Planificador y Proceso)

-   `kernel/proc.h`: Se agregaron los nuevos campos a `struct proc`:
    -   `int tickets;`
    -   `int cpu_slices;`
-   `kernel/proc.c`:
    -   En `allocproc()`: Se inicializaron los valores por defecto `p->tickets = 100;` y `p->cpu_slices = 0;`.
    -   Se añadió una función `static uint random(uint max)` para generar números pseudo-aleatorios usando un LCG simple basado en `ticks`.
    -   En `scheduler()`: Se reemplazó completamente la lógica Round-Robin por el algoritmo de lotería descrito anteriormente (calcular total, generar aleatorio, encontrar ganador, incrementar `cpu_slices` y ejecutar).
    -   En `kexit()`: Se añadió un bloque `printf` que imprime los `tickets` y `cpu_slices` del proceso justo antes de pasar a estado `ZOMBIE`. Esto fue crucial para monitorear y verificar la proporcionalidad en las pruebas.

### Parte III: Programa de Prueba

-   `user/demo.c`: Programa de prueba principal. Crea 10 procesos hijos usando `fork()`. Cada hijo `i` recibe `50 * (i + 1)` *tickets* usando `settickets()`. Luego, cada hijo ejecuta un bucle de trabajo intensivo (`work()`) para consumir tiempo de CPU.
-   `Makefile`: Se añadió `_demo` a la lista `UPROGS` para que el programa de prueba fuera compilado y estuviera disponible en la shell de xv6.

---

## Pruebas y Dificultades

Durante la implementación, surgieron varios desafíos técnicos que requirieron depuración:

1.  **Errores de Compilación del Kernel:** Al implementar `scheduler()`, se produjo un error al llamar a `swtch(&c->scheduler, &p->context)`. La corrección fue usar `&c->context`, ya que el contexto del *scheduler* se almacena en la estructura `cpu`, no en un campo inexistente `scheduler`. Adicionalmente, al implementar `sys_settickets()`, se comprobó erróMneamente el valor de retorno de `argint()`, que en esta versión de xv6 es `void`. Se corrigió llamando a `argint(0, &n);` sin comprobar un valor de retorno.

2.  **Resultados de Prueba No Proporcionales:** Este fue el desafío principal. En las primeras ejecuciones de `demo`, los resultados eran ilógicos: un proceso con 50 *tickets* obtenía ~50 `cpu_slices`, y un proceso con 500 *tickets* (10 veces más) obtenía solo ~52 `cpu_slices`. La proporcionalidad estaba rota.

    -   **Diagnóstico:** El problema no estaba en el *scheduler*, sino en el bucle de trabajo (`work()`) del programa de prueba `demo.c`. El bucle `for (long i = 0; i < 2000000000; i++)` estaba causando un **desbordamiento de entero** (*integer overflow*). En el compilador de xv6, `long` es de 32 bits, y el número 2 mil millones está al borde del límite, causando que el bucle terminara casi instantáneamente.
    -   **Solución:** Se cambió el tipo de la variable `i` a `unsigned long long` (que garantiza 64 bits) y se especificó el número como `5000000000ULL` (usando el sufijo `ULL`). Esto forzó la aritmética de 64 bits, evitando el desbordamiento y permitiendo que la prueba se ejecutara el tiempo suficiente para que la naturaleza probabilística del *scheduler* se estabilizara y mostrara la proporcionalidad esperada.

## Posibles Problemas del Lottery Scheduling

Aunque es un planificador justo, el *Lottery Scheduling* tiene desventajas teóricas y prácticas:

1.  **Incertidumbre a Corto Plazo:** Al ser probabilístico, no garantiza justicia en ventanas de tiempo cortas. Un proceso importante (con muchos *tickets*) podría, por mala suerte, no ser elegido durante un tiempo, lo cual es inaceptable para sistemas de tiempo real.
2.  **Inversión de Prioridad con I/O:** Un proceso de alta prioridad (muchos *tickets*) que está listo para ejecutarse puede verse bloqueado por un proceso de baja prioridad (pocos *tickets*) que actualmente posee un recurso (como un *lock* de disco). El proceso de baja prioridad necesita CPU para liberar el recurso, pero pierde constantemente la lotería, "matando de hambre" (starvation) al proceso de alta prioridad.
3.  **Manejo de la Inflación de Tickets:** ¿Cómo se asignan los *tickets*? Si los procesos pueden asignarse *tickets* a sí mismos (como en nuestra implementación), un proceso malicioso o mal programado podría darse a sí mismo una cantidad astronómica de *tickets*, acaparando la CPU y causando *starvation* al resto del sistema.
4.  **Complejidad de Cálculo:** Aunque es más simple que otros algoritmos, requiere recalcular el `total_tickets` o (en implementaciones más avanzadas) mantener una estructura de datos (como un árbol) actualizada cada vez que un proceso cambia de estado (RUNNABLE, SLEEPING, etc.), lo cual añade sobrecarga.

---

## Conclusiones

Se implementó con éxito un planificador de *Lottery Scheduling* en xv6. La creación de la *system call* `settickets` nos permitió introducir parámetros dinámicos para los procesos, mientras que la modificación del planificador `scheduler()` representó el desafío central, al reemplazar el esquema Round-Robin por un mecanismo probabilístico basado en sorteo.

En el camino, pudimos reforzar la importancia de coordinar los cambios en la lógica del kernel (`kernel/proc.c`, `kernel/sysproc.c`) y en la interfaz de usuario (`user/user.h`, `user/usys.pl`). Asimismo, las variables agregadas `tickets` y `cpu_slices` nos permitieron experimentar directamente con la relación entre asignación de CPU y los parámetros definidos por el usuario.

El desafío más significativo no estuvo en el kernel, sino en la validación: crear un programa de prueba (`demo.c`) que no solo funcionara, sino que evitara errores sutiles como el *integer overflow* para probar *correctamente* la proporcionalidad del *scheduler*. Esta tarea nos permitió profundizar en el rol del planificador y en el funcionamiento de las *syscalls* en xv6.