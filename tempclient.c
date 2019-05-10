#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "client.h"

int main (int argc, char *argv[]) {
  struct sockaddr_in addr;
  int addrlen, sock, cnt;
  struct ip_mreq mreq;
  char message[50];

  /* TCP */
  char buf[12];              /* data buffer for sending & receiving */
  struct hostent *hostnm;    /* server host name information        */
  struct sockaddr_in server; /* server address                      */
  int s;                     /* client socket                       */
  struct clients_connectes nouveau_client;
  char name[1024];
  gethostname(name, 1023);

  printf("Veuillez choisir votre pseudo : ");
  scanf("%s", message);

  /* The host name is the first argument. Get the server address. */
  hostnm = gethostbyname(name);

  if (hostnm == (struct hostent *) 0) {
      fprintf(stderr, "Gethostbyname failed\n");
      exit(2);
  }

  /* Get a stream socket. */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
         perror("Socket()");
         exit(3);
  }

  /* Put a message into the buffer. */
  nouveau_client.pseudo = message;
  /*
   * Put the server information into the server structure.
   * The port must be put into network byte order.
   */
  server.sin_family      = AF_INET;
  server.sin_port        = htons(PORT_TCP);
  server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

  nouveau_client.infos = server;
  /* FIN TCP */

  /* set up socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT_UDP);
  addrlen = sizeof(addr);

  addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

	 printf("Requête Multicast envoyé au Multicast.\n");
	 cnt = sendto(sock, (struct sockaddr_in*) &nouveau_client, sizeof(nouveau_client), 0, (struct sockaddr *) &addr, addrlen);
	 if (cnt < 0) {
 	    perror("sendto");
	    exit(1);
	}

  /*********************************************************************/
  /*                                                                   */
  /*                                                                   */
  /*                          PARTIE TCP                               */
  /*                                                                   */
  /*                                                                   */
  /*********************************************************************/

   /* Connect to the server. */
   if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
       perror("Connect() client");
       exit(4);
   }

   if (send(s, buf, sizeof(buf), 0) < 0) {
       perror("Send()");
       exit(5);
   }

   /* The server sends back the same message. Receive it into the buffer. */
   if (recv(s, buf, sizeof(buf), 0) < 0) {
       perror("Recv()");
       exit(6);
   }

   printf("Message reçu du Serveur TCP : %s\n", nouveau_client.pseudo);

   /* Close the socket. */
   close(s);

   printf("Client Ended Successfully\n");

  return 0;
}
