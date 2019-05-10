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
  struct sockaddr_in    localSock, groupSock;
  struct in_addr        localInterface;
  struct ip_mreq        group;
  int                   socketUDP, socketTCP, socket_service;
  char                  databuf[TAILLE_BUFFER];
  struct hostent        *host;
  char                  name[256];
  struct sockaddr_in    tcp_addr;
  char*                 donnees_multicast;
  int                   identifiant = 0;
  // adresse socket coté client
  static struct sockaddr_in addr_client;
  // adresse socket locale
  static struct sockaddr_in addr_serveur;

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


  /**************** PARTIE UDP ****************/

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
  if (setsockopt(socketUDP, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (char *)&group, sizeof(group)) < 0) {
    perror("adding multicast group");
    close(socketUDP);
    exit(1);
  }

  // configuration socket écoute : 5 connexions max en attente
  if (listen(socketTCP, 5) == -1) {
    perror("erreur listen");
    exit(1);
  }

  int lg_addr = sizeof(struct sockaddr_in);

    // configuration socket écoute : 5 connexions max en attente
    if (listen(socketTCP, 5) == -1) {
      perror("erreur listen");
      exit(1);
    }

    // on attend la connexion du client
    lg_addr = sizeof(struct sockaddr_in);

    while (1) {
      socket_service = accept(socket_ecoute, (struct sockaddr *)&addr_client, &lg_addr);

      if (socket_service == -1) {
        perror("erreur accept");
        exit(1);
      }

      // la connexion est établie, on attend les données envoyées par le client
      nb_octets = read(socket_service, message, TAILLEBUF);

      // affichage du message reçu
      chaine_recue = (char *)malloc(nb_octets * sizeof(char));
      memcpy(chaine_recue, databuf, nb_octets);
      printf("%s\n", databuf);
      // on envoie la réponse au client
      write(socket_service, databuf, strlen(databuf)+1);
    }

    // on ferme les sockets
    close(socket_service);
    close(socket_ecoute);

  close(socketUDP);
  close(socketTCP);
}
