#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include "client.h"

int main (int argc, char *argv[]) {
  struct in_addr        localInterface;
  struct sockaddr_in    groupSock;
  int                   sd;
  int                   datalen;  /* recuperer le nom du hote */
  struct hostent        *host;
  char                  databuf[1024];

  printf("Veuiller saisir votre identifiant : ");
  scanf("%s", databuf);

  /*
   * Create a datagram socket on which to send.
   */
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  /*
   * Initialize the group sockaddr structure with a
   * group address of 225.1.1.1 and port 5555.
   */
  memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
  groupSock.sin_port = htons(5555);

  /*
   * Disable loopback so you do not receive your localSockown datagrams.
   */
  {
    char loopch=0;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
                   (char *)&loopch, sizeof(loopch)) < 0) {
      perror("setting IP_MULTICAST_LOOP:");
      close(sd);
      exit(1);
    }
  }

  /*
   * Set local interface for outbound multicast datagrams.
   * The IP address specified must be associated with a local,
   * multicast-capable interface.
   */
  localInterface.s_addr = inet_addr(MULTICAST_IP);
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&localInterface,
                 sizeof(localInterface)) < 0) {
    perror("setting local interface");
    exit(1);
  }

  /*
   * Send a message to the multicast group specified by the
   * groupSock sockaddr structure.
   */
  if (sendto(sd, databuf, TAILLE_BUFFER, 0,
             (struct sockaddr*)&groupSock,
             sizeof(groupSock)) < 0)
  {
    perror("sending datagram message");
  }

  /*** TCP  ********************************************************/
  /*                                                                   */
  /* Component Name: TCPS                                              */
  /*                                                                   */
  /*                                                                   */
  /* Copyright:    Licensed Materials - Property of LA FAC             */
  /*** IBMCOPYR ********************************************************/

   char buf[12];              /* data buffer for sending & receiving */
   struct hostent *hostnm;    /* server host name information        */
   struct sockaddr_in server; /* server address                      */
   int s;                     /* client socket                       */
   char name[1024];
   gethostname(name, 1023);

   /* The host name is the first argument. Get the server address. */
   hostnm = gethostbyname(name);

   if (hostnm == (struct hostent *) 0) {
       fprintf(stderr, "Gethostbyname failed\n");
       exit(2);
   }

   /* Put a message into the buffer. */
   strcpy(buf, "the message");

   /*
    * Put the server information into the server structure.
    * The port must be put into network byte order.
    */
   server.sin_family      = AF_INET;
   server.sin_port        = htons(PORT);
   server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

   /* Get a stream socket. */
   if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
       perror("Socket()");
       exit(3);
   }

   /* Connect to the server. */
   if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
       perror("Connect()");
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

   /* Close the socket. */
   close(s);

   printf("Client Ended Successfully\n");

  return 0;
}
