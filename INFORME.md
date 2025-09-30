# TAREA 1 Sistemas operativos Sec 2

## **Integrantes**

- **Martin Peralta** <br>
  <martperalta@alumnos.uai.cl>

- **Cristobal Zeballos** <br>
  <czeballos@alumnos.uai.cl>

## Descripción: Se coloco la nueva llamada al sistema `getppid(void)` en xv6, que retorna el PID del proceso de padre. Si el proceso no tiene padre retorna `-1`. Esto permiten que los programas de usuario usen servicios del sistema operativo para que se puedan crear procesos o leer archivos.


## Archivos modificados para implementar getpidd(void)
- `kernel/syscall.h`: se agregó la constante `SYS_getppid`.
- `kernel/syscall.c`: se agregó la entrada correspondiente en la tabla de llamadas.
- `kernel/sysproc.c`: se implementó la función `sys_getppid` .
- `user/user.h`: se agregó el prototipo de `int getppid(void);`.
- `user/usys.pl`: se añadió la entrada para generar el wrapper en `usys.S`para que sea mas facul desde el usuario.
- `user/yosoytupadre.c`: programa de prueba que muestra el PID del proceso y de su padre.

## Conclusiones En esta tarea aprendimos a agregar llamadas al sistema en xv6, teniendo en cuenta la importancia de modificar tanto el kernel como la interfaz de usuario para que todo funcione en conjunto y bien. Logramos compilar, ejecutar y validar la nueva syscall, aunque lo más complicado fue ubicar el lugar exacto donde se debían hacerse los cambios, ya que a veces no funcionaba por estar mal implementados. En el proceso pudimos comprender cómo las llamadas al sistema actúan como puente entre el usuario y el kernel, permitiendo que los programas accedan de manera controlada a los recursos del sistema operativo.