#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "client.h"

#define TRUE   1
#define FALSE  0
#define MULTICAST_GROUP "226.1.2.3"
#define MULTICAST_IP "127.0.1.1"

int main (int argc, char *argv[]) {
  int sock, status, socklen;
  char buffer[TAILLE_BUFFER];
  struct sockaddr_in saddr;
  struct in_addr iaddr;
  unsigned char ttl = 3;
  unsigned char one = 1;
  int buffer_valid = FALSE;

  // set content of struct saddr and imreq to zero
  memset(&saddr, 0, sizeof(struct sockaddr_in));
  memset(&iaddr, 0, sizeof(struct in_addr));

  // open a UDP socket
  sock = socket(PF_INET, SOCK_DGRAM, 0);
  if ( sock < 0 )
    perror("Error creating socket"), exit(0);

  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(0); // Use the first free port
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
  status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

  if ( status < 0 )
    perror("Error binding socket to interface"), exit(0);

  iaddr.s_addr = INADDR_ANY; // use DEFAULT interface

  // Set the outgoing interface to DEFAULT
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
             sizeof(struct in_addr));

  // Set multicast packet TTL to 3; default TTL is 1
  setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
             sizeof(unsigned char));

  // send multicast traffic to myself too
  status = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
                      &one, sizeof(unsigned char));

  // set destination multicast address
  saddr.sin_family = PF_INET;
  saddr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
  saddr.sin_port = htons(PORT);

  // put some data in buffer


  while (!buffer_valid) {
    printf("Veuillez choisir votre pseudo : ");
    fgets(buffer, TAILLE_BUFFER, stdin);

    if (strlen(buffer) < 6) {
      printf("Veuillez avoir 6 caracteres minimum\n\n");
    } else {
      buffer_valid = TRUE;
    }
  }

  socklen = sizeof(struct sockaddr_in);
  // receive packet from socket
  status = sendto(sock, buffer, strlen(buffer), 0,
                    (struct sockaddr *)&saddr, socklen);
  if (status < 0) {
    perror("sendto()");
  }

  //printf("message sent.\n");
  memset(buffer, 0, TAILLE_BUFFER);
  recvfrom(sock, buffer, TAILLE_BUFFER, 0,
                    (struct sockaddr *)&saddr, &socklen);
  printf("%s\n", buffer);

  // shutdown socket
  shutdown(sock, 2);
  // close socket
  close(sock);

  return 0;

}
