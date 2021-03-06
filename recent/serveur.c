#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "serveur.h"

#define TRUE   1
#define FALSE  0
#define MULTICAST_GROUP "225.1.1.1"
#define MULTICAST_IP "127.0.1.1"

int main (int argc, char *argv[]) {
  int sock, status, socklen;
  char buffer[TAILLE_BUFFER];
  struct sockaddr_in saddr;
  struct ip_mreq imreq;
  int identifier = 1;

  // set content of struct saddr and imreq to zero
  memset(&saddr, 0, sizeof(struct sockaddr_in));
  memset(&imreq, 0, sizeof(struct ip_mreq));

  // open a UDP socket
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if ( sock < 0 )
    perror("Error creating socket"), exit(0);

  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(PORT); // listen on port 8080
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
  status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

  if ( status < 0 )
    perror("Error binding socket to interface"), exit(0);

  imreq.imr_multiaddr.s_addr = inet_addr("226.0.0.1");
  imreq.imr_interface.s_addr = INADDR_ANY; // use DEFAULT interface

  // JOIN multicast group on default interface
  status = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
             (const void *)&imreq, sizeof(struct ip_mreq));

  while (TRUE) {
    socklen = sizeof(struct sockaddr_in);

    // receive packet from socket
    status = recvfrom(sock, buffer, TAILLE_BUFFER, 0,
                      (struct sockaddr *)&saddr, &socklen);
    printf("%s\n", buffer);

    memset(buffer, 0, TAILLE_BUFFER);
    buffer[0] = identifier + '0';
    identifier++;

    printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , sock , inet_ntoa(saddr.sin_addr) , ntohs(saddr.sin_port));

    sendto(sock, buffer, strlen(buffer), 0,
                      (struct sockaddr *)&saddr, socklen);
  }

  // shutdown socket
  shutdown(sock, 2);
  // close socket
  close(sock);

  return 0;

  return 0;
}
