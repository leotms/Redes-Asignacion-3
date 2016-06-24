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
      printf("Guadado en i = %d\n",i);
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
      strftime(fecha,sizeof(fecha),"%D %T", &vehiculo->tiempoEntrada);
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
  printf("Retirando Vehiculo con placa : %d\n",*placa);
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] != NULL && estacionamiento[i]->placa == *placa){
      vehiculo = estacionamiento[i];
      estacionamiento[i] = NULL;
      break;
    }
  }
  return vehiculo;
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
void * procesar_pdu(PDU* pdu_entrante, REG_VEHICULO * estacionamiento[], int * puestos_ocupados){
	printf("Hello \n");
	/*Apuntador al vehiculo a ingresar o sacar del estacionamiento*/
	REG_VEHICULO * vehiculo;

	/*Este sera el tiempo de entrada o salida del vehiculo segun corresponda*/
	time_t t = time(NULL);
	//struct tm local_tm;
	//struct tm *tmp = localtime_r(&t,&local_tm);

	if (pdu_entrante->tipo_paq == 'e'){
		/*Como la operacion es de entrada, reservamos la memoria para el nuevo
		 * vehiculo y le asignamos los valores correspondientes.*/
		vehiculo = (REG_VEHICULO *) malloc(sizeof(REG_VEHICULO));
		memcpy(&vehiculo->placa, &pdu_entrante->placa, sizeof(int));
		localtime_r(&t,&vehiculo->tiempoEntrada);
		//vehiculo->tiempoEntrada = fecha_hora;

		/*Ingresamos el vehiculo al estacionamiento*/
		ingresar_vehiculo(estacionamiento,vehiculo);

		/*Imprimimos el vehiculo solo para verificar*/
		imprimir_estacionamiento(estacionamiento);

		/*Incrementamos el numero de puestos*/
		*puestos_ocupados = *puestos_ocupados + 1;

		printf("Numero de puestos : %d\n", *puestos_ocupados);


	} else if (pdu_entrante->tipo_paq == 's') {
		/*En este caso, vehiculo corresponde el vehiculo que sacaremos del estacionamiento*/
		vehiculo = retirar_vehiculo(estacionamiento, &pdu_entrante->placa);

		/*Si vehiculo retorna como NULL, entonces no exitia o hubo un error.*/
		if (vehiculo == NULL) {
			printf("El vehiculo con placa %d no existe o hubo un error.\n", pdu_entrante->placa);
			/* Aqui le enviamos algo al cliente*/
		} else {
			printf("El vehiculo retirado es:  %d\n", vehiculo->placa);
			/*Imprimimos el vehiculo solo para verificar*/
			imprimir_estacionamiento(estacionamiento);
			/*Decrementamos el numero de puestos*/
			*puestos_ocupados = *puestos_ocupados - 1;
			printf("Numero de puestos : %d\n", *puestos_ocupados);

			/*Liberamos la memoria del vehiculo*/
			free(vehiculo);
		}
	}
}
//
// struct tm * tiempoLocal() {
// 	struct tm newtime;
// 	time_t ltime;
// 	char buf[50];
//
// 	ltime=time(&ltime);
// 	return &newtime;
// }

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
	//memset(&datos_servidor,0,sizeof(struct sockaddr_in));
	//memset(&datos_cliente ,0,sizeof(struct sockaddr_in));

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
		//fflush(stdout);

		if ((numero_bytes = recvfrom(socketfd, pdu_entrante, sizeof(PDU), 0,
									(struct sockaddr*) &datos_cliente,
									(socklen_t *) &tam_direccion)) == -1){
			error("Error recibiendo datos del cliente.");
		}


		/* Imprimimos en pantalla el mensaje recibido.*/
		printf("\nMensaje recibido de %s\n",inet_ntoa(datos_cliente.sin_addr));
		printf("Longitud del PDU en bytes %d\n",numero_bytes);
		printf("\nTipo de paquete: %c\n", pdu_entrante-> tipo_paq);
		printf("Orígen del paquete: %d\n", pdu_entrante-> fuente);
	  printf("Placa del vehiculo: %d\n", pdu_entrante-> placa);

    /* Hora y Fecha del sistema */
		/* -------------------------- ERROR ----------------------------------- */
    char fecha[18];
    time_t t;
		time(&t);
    struct tm tmp;
    localtime_r(&t,&tmp);
    strftime(fecha,sizeof(fecha),"%D %T",&tmp);
		/* -------------------------- ERROR ----------------------------------- */
		/* EL ERROR ESTA CUANDO SE PIDE EL LOCALTIME. Lo corro en todos lados y nada.*/
		/* Quizas podamos cambiar el string del PDU por un solo un time_t e imprimirlo del lado del cliente?*/

    int c, m;

    //c = (int *)malloc(sizeof(int));
    c = 100;
    //m = (int *)malloc(sizeof(int));
    m = 80;

    /* Datos a enviar/Recibir */

    pdu_salida-> fuente = true;
    pdu_salida-> puesto = true;
    pdu_salida-> placa = pdu_entrante->placa;
    strcpy(pdu_salida->fecha_hora,fecha);
    pdu_salida-> codigo = c;

    //registrar(bitacora_entrada,pdu_entrante);
		// if (numero_bytes = sendto(socketfd, pdu_salida, sizeof(PDU), 0,
		// 						(struct sockaddr*) &datos_cliente,
		// 						sizeof(struct sockaddr)) == -1){
		// 		error("Error enviando datos al cliente.");
		// }

		procesar_pdu(pdu_entrante,estacionamiento,&puestos_ocupados);

	}

	close(socketfd);
	exit(0);

}
