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
  struct in_addr        localInterface;
  struct sockaddr_in    groupSock;
  int                   socketUDP, socketTCP;
  char                  databuf[TAILLE_BUFFER];
  struct sockaddr_in    addr;
  struct hostent        *host;
  char                  name[256];

  /* ------------------------------------------------------------*/
  /*                                                             */
  /* Send Multicast Datagram code example.                       */
  /*                                                             */
  /* ------------------------------------------------------------*/

  printf("Veuiller saisir votre identifiant : ");
  scanf("%s", databuf);

  /* creer socket TCP */
  socketTCP = socket(AF_INET, SOCK_STREAM, 0);
  if (socketTCP < 0) {
    perror("opening TCP socket");
    exit(1);
  }

  /* recuperer le nom du hote */
  gethostname(name, 1023);
  host = gethostbyname(name);

  /* assigner port et l'adresse IP */
  bzero((char *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);

  /*
   * Create a datagram socket on which to send.
   */
  socketUDP = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketUDP < 0) {
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
  groupSock.sin_port = htons(PORT);

  /*
   * Disable loopback so you do not receive your own datagrams.
   */
  {
    char loopch=0;

    if (setsockopt(socketUDP, IPPROTO_IP, IP_MULTICAST_LOOP,
                   (char *)&loopch, sizeof(loopch)) < 0) {
      perror("setting IP_MULTICAST_LOOP:");
      close(socketUDP);
      exit(1);
    }
  }

  /*
   * Set local interface for outbound multicast datagrams.
   * The IP address specified must be associated with a local,
   * multicast-capable interface.
   */
  localInterface.s_addr = inet_addr(MULTICAST_IP);
  if (setsockopt(socketUDP, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&localInterface,
                 sizeof(localInterface)) < 0) {
    perror("setting local interface");
    exit(1);
  }

  strcat(databuf, "\n");
  strcat(databuf, inet_ntoa(addr.sin_addr));

  /*
   * Send a message to the multicast group specified by the
   * groupSock sockaddr structure.
   */
  if (sendto(socketUDP, databuf, sizeof(databuf), 0,
             (struct sockaddr*)&groupSock,
             sizeof(groupSock)) < 0)
  {
    perror("sending datagram message");
  } else {
    printf("datagram sent to Multicast from client.\n");
  }

  while (1) {
    /* code */
  }

  close(socketUDP);
  close(socketTCP);

  return 0;
}
