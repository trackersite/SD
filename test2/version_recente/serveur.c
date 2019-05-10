#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "serveur.h"

int main (int argc, char *argv[]) {
  struct sockaddr_in addr;
  int addrlen, sock, cnt;
  struct ip_mreq mreq;
  char message[50];
  /* TCP */
  char buf[12];              /* buffer for sending & receiving data */
  struct sockaddr_in client; /* client address information          */
  struct sockaddr_in server; /* server address information          */
  int s;                     /* socket for accepting connections    */
  int ns;                    /* socket connected to client          */
  int namelen;               /* length of client name               */

  /*
   * Get a socket for accepting connections.
   */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      perror("Socket()");
      exit(2);
  }

  /*
   * Bind the socket to the server address.
   */
  server.sin_family = AF_INET;
  server.sin_port   = htons(PORT_TCP);
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
      perror("Bind()");
      exit(3);
  }

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

  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    perror("bind");
	  exit(1);
  }

  mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
	 perror("setsockopt mreq");
	 exit(1);
  }

  while (1) {
 	 if ((recvfrom(sock, message, sizeof(message), 0, (struct sockaddr *) &addr, &addrlen) < 0)) {
     perror("recvfrom() server");
   }
	 printf("%s: Message réçu du Client par Multicast = \"%s\"\n", inet_ntoa(addr.sin_addr), message);


   /*
    * Listen for connections. Specify the backlog as 1.
    */
   if (listen(s, 1) != 0)
   {
       perror("Listen()");
       exit(4);
   }

   /*
    * Accept a connection.
    */
   namelen = sizeof(client);
   if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
   {
       perror("Accept()");
       exit(5);
   }

   /*
    * Receive the message on the newly connected socket.
    */
   if (recv(ns, buf, sizeof(buf), 0) == -1)
   {
       perror("Recv()");
       exit(6);
   }

   memset(buf, 0, TAILLE_BUFFER);
   strcpy(buf, "Hello");

   /*
    * Send the message back to the client.
    */
   if (send(ns, buf, sizeof(buf), 0) < 0)
   {
       perror("Send()");
       exit(7);
   }

   printf("Message envoyé au client.\n");

  }

  /*********************************************************************/
  /*                                                                   */
  /*                                                                   */
  /*                          PARTIE TCP                               */
  /*                                                                   */
  /*                                                                   */
  /*********************************************************************/

   close(ns);
   close(s);

   printf("Server ended successfully\n");

   return 0;
}
