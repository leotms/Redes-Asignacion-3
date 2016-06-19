/* Aplicacion Servidor para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes 10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 18/06/16
*/

/* Archivo de cabecera*/
#include "servidor.h"

/* Programa Principal*/
int main(int argc, char *argv[]) {

    /*guardaran las los datos del cliente y servidor*/
    struct sockaddr_in datos_servidor, datos_cliente;

    /* socketfd: descriptor del socket.
    * tam_direccion: tama#o de la estructura sockaddr_in
    * numero_bytes: himero de bytes recibidos.
    */
    int socketfd, tam_direccion, numero_bytes;

    /* Contendra el PDU entrante */
    char pdu_entrante[MAX_PDU_LENGTH];

    /*creamos el socket*/
    printf("Creando el socket.\n");
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      perror("No se pudo crear el socket.");
      exit(-1);
    }

    /* Asignamos los datos del servidor*/
    datos_servidor.sin_family = AF_INET;
    datos_servidor.sin_port = htons(PUERTO_SERVIDOR);
    datos_servidor.sin_addr.s_addr = INADDR_ANY; // Para que escuche de cualquier IP.
    bzero(&(datos_servidor.sin_zero),8);

    /*Unimos el socket con los datos del servidor*/

    printf("Uniendo el socket con la direccion.\n");
    if (bind(socketfd, (struct sockaddr*)&datos_servidor, sizeof(struct sockaddr)) == -1) {
      perror("Error uniendo el socket con los datos del servidor.");
      exit(-1);
    }

    printf("Esperando datos del cliente: \n");
    while(1){

      if ((numero_bytes = recvfrom(socketfd, pdu_entrante, MAX_PDU_LENGTH, 0,
                                  (struct sockaddr*) &datos_cliente,
                                  (socklen_t *) &tam_direccion)) == -1){
         perror("Error recibiendo datos del cliente.");
         exit(-1);
      }

      /* Imprimimos en pantalla el mensaje recibido.*/
      printf("Mensaje recibido de %s\n",inet_ntoa(datos_cliente.sin_addr));
      printf("Longitud del PDU en bytes %d\n",numero_bytes);
      pdu_entrante[numero_bytes] = '\0';
      printf("El mensaje es: %s\n",pdu_entrante);
      fflush(stdout);

    }

    close(socketfd);
    exit(0);
    
}
