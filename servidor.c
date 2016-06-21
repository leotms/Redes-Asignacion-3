/* Aplicacion Servidor para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes 10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 18/06/16
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

  
  //op = pdu-> operacion;
  fputc(pdu-> serialID,archivo);
 // fputs(op,archivo);
  fclose(archivo);

}

/* Funcion que verifica y devuelve los argumentos de entrada*/
void * leer_args(int argc, char *argv[], int *numero_puerto,
                       char *bitacora_entrada, char *bitacora_salida){
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
          strcpy(bitacora_salida, argv[i+1]);
        }
    }
}

/* Programa Principal*/
int main(int argc, char *argv[]) {


    /*Almacentaran los argumentos recibidos de la linea de comandos*/
    int numero_puerto;
    char *bitacora_entrada = (char *) malloc(512*sizeof(char));
    char *bitacora_salida  = (char *) malloc(512*sizeof(char));

    /*Leemos los argumentos y los asignamos a las variables respectivas.*/
    leer_args(argc,argv,&numero_puerto, bitacora_entrada, bitacora_salida);
    printf("Este es el archivo de entrada (afuera): %s\n", bitacora_entrada);
    printf("Este es el archivo de salida (afuera): %s\n", bitacora_salida);
    printf("Este es el numero de puerto (afuera): %d\n", numero_puerto );

    /*guardaran las los datos del cliente y servidor*/
    struct sockaddr_in datos_servidor, datos_cliente;

    /* socketfd: descriptor del socket.
    * tam_direccion: tama#o de la estructura sockaddr_in
    * numero_bytes: himero de bytes recibidos.
    */
    int socketfd, tam_direccion, numero_bytes;

    /* Contendra el PDU entrante */
    //char pdu_entrante[MAX_PDU_LENGTH];
    PDU *pdu_entrante;
    pdu_entrante = (PDU *)malloc(sizeof(PDU));

    /*creamos el socket*/
    printf("Creando el socket.\n");
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      error("No se pudo crear el socket.");
    }

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

    printf("Esperando datos del cliente: \n");
    while(1){

      if ((numero_bytes = recvfrom(socketfd, pdu_entrante, MAX_PDU_LENGTH, 0,
                                  (struct sockaddr*) &datos_cliente,
                                  (socklen_t *) &tam_direccion)) == -1){
         error("Error recibiendo datos del cliente.");
      }

      /* Imprimimos en pantalla el mensaje recibido.*/
      printf("\nMensaje recibido de %s\n",inet_ntoa(datos_cliente.sin_addr));
      printf("Longitud del PDU en bytes %d\n",numero_bytes);
      //printf("El mensaje es: %s\n",pdu_entrante);
      printf("\nnumero vehiculo %d\n", pdu_entrante-> serialID);
      printf("operacion %c\n", pdu_entrante-> operacion);
      registrar(bitacora_entrada,pdu_entrante);
      fflush(stdout);

    }

    close(socketfd);
    exit(0);

}
