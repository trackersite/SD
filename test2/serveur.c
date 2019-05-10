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
  struct sockaddr_in    localSock;
  struct ip_mreq        group;
  int                   sd;
  int                   datalen;
  char                  databuf[1024];

    /*
     * Create a datagram socket on which to receive.
     */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
      perror("opening datagram socket");
      exit(1);
    }

    /*
     * Enable SO_REUSEADDR to allow multiple instances of this
     * application to receive copies of the multicast datagrams.
     */
    {
      int reuse=1;

      if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
                     (char *)&reuse, sizeof(reuse)) < 0) {
        perror("setting SO_REUSEADDR");
        close(sd);
        exit(1);
      }
    }

    /*
     * Bind to the proper port number with the IP address
     * specified as INADDR_ANY.
     */
    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(5555);
    localSock.sin_addr.s_addr  = INADDR_ANY;

    if (bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))) {
      perror("binding datagram socket");
      close(sd);
      exit(1);
    }


    /*
     * Join the multicast group 225.1.1.1 on the local 9.5.1.1
     * interface.  Note that this IP_ADD_MEMBERSHIP option must be
     * called for each local interface over which the multicast
     * datagrams are to be received.
     */
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    group.imr_interface.s_addr = inet_addr(MULTICAST_IP);
    if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (char *)&group, sizeof(group)) < 0) {
      perror("adding multicast group");
      close(sd);
      exit(1);
    }

    /*
     * Read from the socket.
     */
    datalen = sizeof(databuf);
    if (read(sd, databuf, datalen) < 0) {
      perror("reading datagram message");
      close(sd);
      exit(1);
    }

    printf("Message from Multicast to TCP = %s\n", databuf);

  /*** TCP  ********************************************************/
  /*                                                                   */
  /* Component Name: TCPS                                              */
  /*                                                                   */
  /*                                                                   */
  /* Copyright:    Licensed Materials - Property of LA FAC             */
  /*** IBMCOPYR ********************************************************/

   unsigned short port;       /* port server binds to                */
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
   server.sin_port   = htons(PORT);
   server.sin_addr.s_addr = INADDR_ANY;

   if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
   {
       perror("Bind()");
       exit(3);
   }

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

   printf("Message from client to TCP = %s\n", buf);

   /*
    * Send the message back to the client.
    */
   if (send(ns, buf, sizeof(buf), 0) < 0)
   {
       perror("Send()");
       exit(7);
   }

   close(ns);
   close(s);

   printf("Server ended successfully\n");

   return 0;
}
