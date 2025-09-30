# TAREA 1 Sistemas operativos Sec 2

## **Integrantes**

- **Martin Peralta** <br>
  <martperalta@alumnos.uai.cl>

- **Cristobal Zeballos** <br>
  <czeballos@alumnos.uai.cl>

## Descripción: Se coloco la nueva llamada al sistema `getppid(void)` en xv6, que retorna el PID del proceso de padre. Si el proceso no tiene padre retorna `-1`.


## Archivos modificados 
- `kernel/syscall.h`: se agregó la constante `SYS_getppid`.
- `kernel/syscall.c`: se agregó la entrada en la tabla de llamadas al sistema.
- `kernel/sysproc.c`: se implementó la función `sys_getppid`.
- `user/user.h`: se agregó el prototipo de `int getppid(void);`.
- `user/usys.pl`: se añadió la entrada para generar el wrapper en `usys.S`.
- `user/yosoytupadre.c`: programa de prueba que muestra el PID del proceso y de su padre.