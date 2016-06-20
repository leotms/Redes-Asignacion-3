/* Aplicacion Cliente para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 19/06/16
*/

/* Archivo de cabecera*/
#include "cliente.h"

/* Imprime en pantalla un error y finaliza la ejecucion*/
void * error(char * mensaje){
  printf("%s\n", mensaje);
  exit(-1);
}

/* Funcion que verifica y devuelve los argumentos de entrada*/
void * leer_args(int argc, char *argv[], char *dominio,
                       int *numero_puerto, char *op,
                       int *id){
     if (argc < 9) {
       error("Error en el numero de argumentos.");
     }

    /* Identificadores de los argumentos */
    char *dom_IP = "-d";
    char *portID = "-p";
    char *operIO = "-c";
    char *vehiID = "-i";

    int i;
    for (i = 1; i < argc; ++i){

        /*Identificamos cada argumento de entrada y lo asignamos donde corresponda*/
        if ((strcmp(argv[i], dom_IP)) == 0){
        	strcpy(dominio, argv[i+1]);
        } else if ((strcmp(argv[i], portID)) == 0) {
        	char *endpt;
        	*numero_puerto = strtol(argv[i+1],&endpt,10);
        } else if ((strcmp(argv[i], operIO)) == 0) {
        	strcpy(op, argv[i+1]);
        } else if ((strcmp(argv[i], vehiID)) == 0) {
        	char *endid;
        	*id = strtol(argv[i+1],&endid,10);
        }
    }
}

/* Programa Principal*/
int main(int argc, char *argv[]) { 


    /*Almacentaran los argumentos recibidos de la linea de comandos*/
    char *dominio = (char *) malloc(512*sizeof(char));
    int numero_puerto;
    char *op= (char *) malloc(sizeof(char));
    int id;

    /*Leemos los argumentos y los asignamos a las variables respectivas.*/
    leer_args(argc,argv,dominio,&numero_puerto,op,&id);
    

    /* sockfd: descriptor del socket.
    * numero_bytes: numero de bytes recibidos.
    */

    int sockfd, numero_bytes;

    /* Guarda la direccion IP y numero de puerto del servidor */ 
    struct sockaddr_in datos_servidor; 
    
    /* Obtiene nombre del host */ 
    struct hostent *host; 
    

    /* Se convierte el hostname a su direccion IP */ 
    if ((host=gethostbyname(dominio)) == NULL) { 
        perror("No se pudo obtener la direccion del host"); 
        exit(1); 
    } 

    /* Creamos el socket */ 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
        perror("socket"); 
        exit(2); 
    } 

    /* Asignamos los datos del servidor */ 
    datos_servidor.sin_family = AF_INET; 
    datos_servidor.sin_port = htons(numero_puerto); 
    datos_servidor.sin_addr = *((struct in_addr *)host->h_addr); 
    bzero(&(datos_servidor.sin_zero), 8); 

    /* Se envían los datos al servidor */ 
    if ((numero_bytes=sendto(sockfd,argv[8],sizeof(int)*4,0,(struct sockaddr *)&datos_servidor,
    sizeof(struct sockaddr))) == -1) { 
        perror("sendto"); 
        exit(2); 
    } 
    printf("enviados %d bytes hacia %s\n",numero_bytes,inet_ntoa(datos_servidor.sin_addr)); 
    printf("numero vehiculo %d\n", id);
    /* cierro socket */ 
    close(sockfd); 
    exit (0); 
}
