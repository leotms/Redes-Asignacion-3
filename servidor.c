/* Aplicacion Servidor para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 22/06/16
*/

/* Archivo de cabecera*/
#include "servidor.h"

/* Imprime en pantalla un error y finaliza la ejecucion*/
void * error(char * mensaje){
	printf("%s\n", mensaje);
	exit(-1);
}

/*Registra un mensaje en la bitacora indicada*/
void * registrar(char *bitacora, PDU *pdu){
	FILE *archivo;
	if ((archivo = fopen(bitacora,"a")) == NULL) {
		error("No se pudo abrir el archivo de bitacora");
	}

	//fputc(pdu-> placa,archivo);
	// fputs(op,archivo);
	fclose(archivo);

}

/*Ingresa un vehiculo en el estacionamiento*/
void *ingresar_vehiculo(REG_VEHICULO * estacionamiento[], REG_VEHICULO * vehiculo){
  int i;
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] == NULL){
      estacionamiento[i] = vehiculo;
      break;
    }
  }
}

/*Imprime todos los vehiculos en un estacionamiento (SOLO PARA DEBUGGING)*/
void * imprimir_estacionamiento(REG_VEHICULO *estacionamiento[]){
  int i;
  REG_VEHICULO * vehiculo;
  char fecha[18];

  printf("IMPRIMIENDO ESTACIONAMIENTO: \n");
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] != NULL){
      vehiculo = estacionamiento[i];
			struct tm tiempo_local;
			localtime_r(&vehiculo->tiempo_entrada,&tiempo_local);
      strftime(fecha,sizeof(fecha),"%D %T", &tiempo_local);
      printf("Vehiculo en puesto %d: \n",i);
      printf(" --Placa: %d\n", vehiculo->placa );
      printf(" --Hora Entrada: %s\n", fecha);
      //free(vehiculo);
      }
  }
}

/* Retira un vehiculo del estacionamiento y retorna un puntero al vehiculo en cuestion NULL si el vehiculo no existe*/
REG_VEHICULO * retirar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa) {
  int i;
  REG_VEHICULO * vehiculo = NULL;
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] != NULL && estacionamiento[i]->placa == *placa){
      vehiculo = estacionamiento[i];
      estacionamiento[i] = NULL;
      break;
    }
  }
  return vehiculo;
}

/* Recibe las horas de entrada y salida y retorna el pago correspondiente del estacionamiento*/
int calcular_pago(time_t hora_entrada, time_t hora_salida){
	int pago;
	double tiempo_total = difftime(hora_salida,hora_entrada);
	if (tiempo_total <= 3600.0){
		pago = 80;
	} else {
		pago = 80;
		tiempo_total = tiempo_total - 3600.0;
		while (tiempo_total > 0.0) {
			pago = pago + 30;
			tiempo_total = tiempo_total - 3600.0;
		}
	}

	printf("Se debe pagar %d Bs.\n",pago);
	return pago;

}

/* Funcion que verifica y devuelve los argumentos de entrada*/
void * leer_args(int argc, char *argv[], int *numero_puerto,
               char *bitacora_entrada, char *bitacora_entrante){
	if (argc < 7) {
		error("Error en el numero de argumentos.");
	}

	/* Identificadores de los argumentos */
	char *portID = "-l";
	char *bitEnt = "-i";
	char *bitSal = "-o";

	int i;
	for (i = 1; i < argc; ++i){

		/*Identificamos cada argumento de entrada y lo asignamos donde corresponda*/
		if ((strcmp(argv[i], portID)) == 0){
			char *endpt;
			*numero_puerto = strtol(argv[i+1],&endpt,10);
		} else if ((strcmp(argv[i], bitEnt)) == 0) {
			strcpy(bitacora_entrada,argv[i+1]);
		} else if ((strcmp(argv[i], bitSal)) == 0) {
			strcpy(bitacora_entrante, argv[i+1]);
		}
	}
}


/* Si hay puestos, procesa el PDU entrante*/
void * procesar_pdu(PDU* pdu_entrante, REG_VEHICULO * estacionamiento[], int * puestos_ocupados,
					PDU * pdu_salida){

	/*Apuntador al vehiculo a ingresar o sacar del estacionamiento*/
	REG_VEHICULO * vehiculo;

	/*Este sera el tiempo de entrada o salida del vehiculo segun corresponda*/
	time_t t = time(NULL);

	if (pdu_entrante->tipo_paq == 'e'){

		/*Falta comprobar si hay puestos en el estacionamiento*/

		/*Como la operacion es de entrada, reservamos la memoria para el nuevo
		 * vehiculo y le asignamos los valores correspondientes.*/
		vehiculo = (REG_VEHICULO *) malloc(sizeof(REG_VEHICULO));
		memcpy(&vehiculo->placa, &pdu_entrante->placa, sizeof(int));
		memcpy(&vehiculo->tiempo_entrada,&t,sizeof(time_t));

		// se le hace strftime a tiempo_entrada para el PDU
		struct tm tiempo_entrada;
		localtime_r(&t,&tiempo_entrada);

		/*Ingresamos el vehiculo al estacionamiento*/
		ingresar_vehiculo(estacionamiento,vehiculo);

		/*Imprimimos el vehiculo solo para verificar*/
		imprimir_estacionamiento(estacionamiento);

		/*Incrementamos el numero de puestos*/
		*puestos_ocupados = *puestos_ocupados + 1;

		printf("Numero de puestos ocupados : %d\n", *puestos_ocupados);

		/* Aqui le enviamos al cliente el mensaje correspondiente*/


        pdu_salida-> tipo_paq = 'e';
        pdu_salida-> fuente = true;
        pdu_salida-> placa = pdu_entrante->placa;

        if (*puestos_ocupados < 3) {
                
            char fecha[18];

            time_t t1 = time(NULL);
            struct tm *tmp;
            tmp = localtime(&t1);

            strftime(fecha,sizeof(fecha),"%D %T",tmp);
            pdu_salida-> puesto = true;
            strcpy(pdu_salida->fecha_hora,fecha);
            pdu_salida-> codigo = 1;

        } else {
            pdu_salida-> puesto = false;         
        }


	} else if (pdu_entrante->tipo_paq == 's') {
		/*En este caso, vehiculo corresponde el vehiculo que sacaremos del estacionamiento*/
		vehiculo = retirar_vehiculo(estacionamiento, &pdu_entrante->placa);

		// Aqui se calcula el tiempo en el que el carro salio del estacionamiento

	  // se le hace strftime a tiempo_salida para el PDU
		struct tm tiempo_salida;
		localtime_r(&t,&tiempo_salida);

		/*Si vehiculo retorna como NULL, entonces no exitia o hubo un error.*/
		if (vehiculo == NULL) {
			printf("El vehiculo con placa %d no existe o hubo un error.\n", pdu_entrante->placa);
			/* Aqui le enviamos al cliente el mensaje correspondiente*/

		} else {
			printf("El vehiculo retirado es:  %d\n", vehiculo->placa);
			/*Imprimimos el vehiculo solo para verificar*/
			imprimir_estacionamiento(estacionamiento);
			/*Decrementamos el numero de puestos*/
			*puestos_ocupados = *puestos_ocupados - 1;
			printf("Numero de puestos ocupados : %d\n", *puestos_ocupados);

			//Se calcula el tipo de pago.
			int monto_a_pagar = calcular_pago(vehiculo->tiempo_entrada,t);

			/* Aqui le enviamos al cliente el mensaje correspondiente*/

	        pdu_salida-> tipo_paq = 's';
	        pdu_salida-> fuente = true;
	        pdu_salida-> placa = pdu_entrante->placa;

			char fecha[18];

            time_t t1 = time(NULL);
            struct tm *tmp;
            tmp = localtime(&t1);

            strftime(fecha,sizeof(fecha),"%D %T",tmp);
            pdu_salida-> puesto = true;
            strcpy(pdu_salida->fecha_hora,fecha);
            pdu_salida-> codigo = 1;
            pdu_salida->monto = monto_a_pagar;

			/*Liberamos la memoria del vehiculo*/
			free(vehiculo);
		}
	}
}

void * salida(PDU * pdu_salida){

	pdu_salida-> fuente = true;
	//pdu_salida-> placa = pdu_entrante->placa;
	char fecha[18];

	time_t t1 = time(NULL);
	struct tm *tmp;
	tmp = localtime(&t1);

	strftime(fecha,sizeof(fecha),"%D %T",tmp);
	pdu_salida-> puesto = true;
	strcpy(pdu_salida->fecha_hora,fecha);
	pdu_salida-> codigo = 1;

}

void * salida1 (int m){
	m = 1000;
}

/* Programa Principal*/
void main(int argc, char *argv[]) {

	/* Inicializamos el estacionamiento con punteros a NULL.*/
	REG_VEHICULO * estacionamiento[MAX_PUESTOS] = {NULL};

	/*Numero de pustos ocupados en el estacionamiento*/
	int puestos_ocupados = 0;

	/*Almacentaran los argumentos recibidos de la linea de comandos*/
	int numero_puerto;
	char *bitacora_entrada = (char *) malloc(512*sizeof(char));
	char *bitacora_entrante  = (char *) malloc(512*sizeof(char));

	/*Leemos los argumentos y los asignamos a las variables respectivas.*/
	leer_args(argc,argv,&numero_puerto, bitacora_entrada, bitacora_entrante);
	printf("Este es el archivo de entrada (afuera): %s\n", bitacora_entrada);
	printf("Este es el archivo de salida (afuera): %s\n", bitacora_entrante);
	printf("Este es el numero de puerto (afuera): %d\n", numero_puerto );

	/*guardaran las los datos del cliente y servidor*/
	struct sockaddr_in datos_servidor, datos_cliente;

	/* socketfd: descriptor del socket.
	* tam_direccion: tama#o de la estructura sockaddr_in
	* numero_bytes: himero de bytes recibidos.
	*/
	int socketfd, tam_direccion, numero_bytes;

	/* Contendra el PDU entrante/saliente */

	PDU *pdu_entrante;
	pdu_entrante = (PDU *)malloc(sizeof(PDU));

	PDU *pdu_salida;
	pdu_salida = (PDU *)malloc(sizeof(PDU));

	/*creamos el socket*/
	printf("Creando el socket.\n");
	if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		error("No se pudo crear el socket.");
	}

	memset((char *) &datos_servidor, 0, sizeof(datos_servidor));

	/* Asignamos los datos del servidor*/
	datos_servidor.sin_family = AF_INET;
	datos_servidor.sin_port = htons(numero_puerto);
	datos_servidor.sin_addr.s_addr = INADDR_ANY; // Para que escuche de cualquier IP.
	bzero(&(datos_servidor.sin_zero),8);

	/*Unimos el socket con los datos del servidor*/

	printf("Uniendo el socket con la direccion.\n");
	if (bind(socketfd, (struct sockaddr*)&datos_servidor, sizeof(struct sockaddr)) == -1) {
		error("Error uniendo el socket con los datos del servidor.");
	}

	while(1){

		printf("\nEsperando datos del cliente...\n");

		if ((numero_bytes = recvfrom(socketfd, pdu_entrante, sizeof(PDU), 0,
									(struct sockaddr*) &datos_cliente,
									(socklen_t *) &tam_direccion)) == -1){
			error("Error recibiendo datos del cliente.");

		} else {

			int size_1 = sizeof(pdu_salida);
			int size_2 = sizeof(PDU);

			printf("\nMensaje recibido de %s\n",inet_ntoa(datos_cliente.sin_addr));
			printf("Longitud del PDU en bytes %d\n",numero_bytes);
			printf("\nTipo de paquete: %c\n", pdu_entrante-> tipo_paq);
			printf("Orígen del paquete: %d\n", pdu_entrante-> fuente);
			printf("Placa del vehiculo: %d\n", pdu_entrante-> placa);

			procesar_pdu(pdu_entrante,estacionamiento,&puestos_ocupados,pdu_salida);

//			salida(pdu_salida);

			int m;
			m = 50;
			salida1(m);

			if (numero_bytes = sendto(socketfd,pdu_salida,sizeof(PDU),0,
									(struct sockaddr*) &datos_cliente,
									sizeof(struct sockaddr)) == -1){
				error("Error enviando datos al cliente.");
			}

		}


		/* Imprimimos en pantalla el mensaje recibido.*/
		// printf("\nMensaje recibido de %s\n",inet_ntoa(datos_cliente.sin_addr));
		// printf("Longitud del PDU en bytes %d\n",numero_bytes);
		// printf("\nTipo de paquete: %c\n", pdu_entrante-> tipo_paq);
		// printf("Orígen del paquete: %d\n", pdu_entrante-> fuente);
		// printf("Placa del vehiculo: %d\n", pdu_entrante-> placa);

		//registrar(bitacora_entrada,pdu_entrante);
		//
		// procesar_pdu(pdu_entrante,estacionamiento,&puestos_ocupados,pdu_salida);
		//
		// salida(pdu_salida);
		//
		// int m;
		// m = 50;
		// salida1(m);

/* COMENTAR ESTE BLOQUE CUANDO UTILICES CUALQUIERA DE LAS FUNCIONES ANTERIORES */
		//
		// pdu_salida-> fuente = true;
		// pdu_salida-> placa = pdu_entrante->placa;
		// char fecha[18];
		//
		// time_t t1 = time(NULL);
		// struct tm *tmp;
		// tmp = localtime(&t1);
		//
		// strftime(fecha,sizeof(fecha),"%D %T",tmp);
		// pdu_salida-> puesto = true;
		// strcpy(pdu_salida->fecha_hora,fecha);
		// pdu_salida-> codigo = 1;

/* DESCOMENTAR EL BLQUE DE ARRIBA SI QUIERES PASAR EL PDU DIRECTO */

		// printf("\n *******PDU SALIDA***********\n");
		// printf("Orígen del paquete: %d\n", pdu_salida-> fuente);
		// printf("Puestos disponibles: %d\n", pdu_salida-> puesto);
		// printf("Placa del vehiculo: %d\n", pdu_salida-> placa);
		// printf("Hora de Entrada/Salida: %s\n", pdu_salida->fecha_hora);
		// printf("Ticket n°: %d\n", pdu_salida-> codigo);


		// if (numero_bytes = sendto(socketfd,pdu_salida,sizeof(PDU),0,
		// 						(struct sockaddr*) &datos_cliente,
		// 						sizeof(struct sockaddr)) == -1){
		// 	error("Error enviando datos al cliente.");
		// }

	}

	close(socketfd);
	exit(0);

}
