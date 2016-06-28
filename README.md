# Asignación 3: Sockets.

Tercera asignación de Redes de Computadores 1.


Aplicación Cliente/Servidor, usando la API Sockets de Berkeley con el
objetivo de diseñar e implementar de un protocolo de comunicación básico.

Programado en lenguaje C.

## Compilación y Ejecución:

Para compilar los archivos y dependencias basta con ejecutar el archivo Makefile

```bash
$ make
```

### Servidor

Ejecutar la línea de comandos:

```bash

./sem_svr -l <puerto_sem_svr> -i <bitácora_entrada> - o <bitácora_salida>

```

Donde:

- **puerto_sem_svr: ** Es el número de puerto local en que el computador central
ofrecerá el servicio.

- **bitácora_entrada: ** Es el nombre y dirección relativa o absoluta de un archivo
de texto que almacena las operaciones de entrada al estacionamiento.

- **bitácora_salida: ** Es el nombre y dirección relativa o absoluta de un archivo
de texto que almacena las operaciones de salida del estacionamiento.

### Cliente

Ejecutar la línea de comandos:

```bash

./sem_cli -d <nombre_módulo_atención> -p <puerto_sem_svr> - c <op> -i <identificación_vehiculo>

```

Donde:
- **nombre_módulo_atención: ** Es el nombre de dominio o la dirección IP
(version 4) del equipo en donde se deberá ejecutar el módulo de atención
centralizada.

- ** puerto_sem_svr: ** Es el número de puerto remoto en que el módulo de
servicio atenderá la comunicación solicitada.

- **op: **Indica si el vehículo va a entrar o salir del estacionamiento, puede tener
dos valores e ó s.

- **identificación_vehículo: ** Es un número serial que identifica únivocamente
un vehículo.



**Autores:**
* [Sahid Reyes](https://github.com/sahidr/).
* [Leonardo Martínez](https://github.com/leotms/).
