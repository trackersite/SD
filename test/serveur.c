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

void multicast(int socketUDP_ecoute) {
  struct sockaddr_in    localSock, groupSock;
  struct ip_mreq        group;
  int                   socketUDP;
  char                  databuf[TAILLE_BUFFER];
  int                   identifiant;
  /*
   * Create a datagram socket on which to receive.
   */
  socketUDP = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketUDP < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  /*
   * Enable SO_REUSEADDR to allow multiple instances of this
   * application to receive copies of the multicast datagrams.
   */
  {
    int reuse=1;

    if (setsockopt(socketUDP, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&reuse, sizeof(reuse)) < 0) {
      perror("setting SO_REUSEADDR");
      close(socketUDP);
      exit(1);
    }
  }

  /*
   * Bind to the proper port number with the IP address
   * specified as INADDR_ANY.
   */
  memset((char *) &localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(PORT);
  localSock.sin_addr.s_addr  = INADDR_ANY;

  if (bind(socketUDP, (struct sockaddr*)&localSock, sizeof(localSock))) {
    perror("binding datagram socket");
    close(socketUDP);
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
  if (setsockopt(socketUDP_ecoute, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                 (char *)&group, sizeof(group)) < 0) {
    perror("adding multicast group");
    close(socketUDP);
    exit(1);
  }

  /*
   * Read from the socket.
   */
  if (read(socketUDP, databuf, sizeof(databuf)) < 0) {
    perror("reading datagram message");
    close(socketUDP);
    exit(1);
  } else {
    printf("Multicast received from client : \n%s\n", databuf);
  }
  memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
  groupSock.sin_port = htons(PORT);

  if (sendto(socketUDP, databuf, sizeof(databuf), 0,
             (struct sockaddr*)&groupSock,
             sizeof(groupSock)) < 0)
  {
    perror("sending datagram message");
  } else {
    printf("datagram sent to TCP Server.\n");
  }
}

int main (int argc, char *argv[]) {
  struct sockaddr_in    localSock, groupSock;
  struct in_addr        localInterface;
  struct ip_mreq        group;
  int                   socketUDP, socketTCP;
  char                  databuf[TAILLE_BUFFER];
  struct hostent        *host;
  char                  name[256];
  struct sockaddr_in    tcp_addr;

  if (socketTCP = creerSocketTCP() <= 0) {
    printf("Could not create TCP socket.\n");
    perror("sock()");
    exit(1);
  }

  /* recuperer le nom du hote */
  gethostname(name, 1023);
  host = gethostbyname(name);

  /* assigner port et l'adresse IP */
  bzero((char *) &tcp_addr, sizeof(tcp_addr));
  tcp_addr.sin_family = AF_INET;
  tcp_addr.sin_port = htons(PORT);
  memcpy(&tcp_addr.sin_addr.s_addr, host->h_addr, host->h_length);

  /*
   * Create a datagram socket on which to receive.
   */
  socketUDP = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketUDP < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  multicast(socketUDP);

  close(socketUDP);
  close(socketTCP);
}
