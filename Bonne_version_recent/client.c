#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "client.h"
#include "common.h"

int main(int argc, char const *argv[]) {
  int sockUDP, status, socklen;
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
  sockUDP = socket(PF_INET, SOCK_DGRAM, 0);
  if ( sockUDP < 0 )
    perror("Error creating socket"), exit(0);

  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(0); // Use the first free port
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
  status = bind(sockUDP, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

  if ( status < 0 )
    perror("Error binding socket to interface"), exit(0);

  iaddr.s_addr = INADDR_ANY; // use DEFAULT interface

  // Set the outgoing interface to DEFAULT
  setsockopt(sockUDP, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
             sizeof(struct in_addr));

  // Set multicast packet TTL to 3; default TTL is 1
  setsockopt(sockUDP, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
             sizeof(unsigned char));

  // send multicast traffic to myself too
  status = setsockopt(sockUDP, IPPROTO_IP, IP_MULTICAST_LOOP,
                      &one, sizeof(unsigned char));

  // set destination multicast address
  saddr.sin_family = PF_INET;
  saddr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
  saddr.sin_port = htons(PORT);

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
  status = sendto(sockUDP, buffer, strlen(buffer), 0,
                    (struct sockaddr *)&saddr, socklen);
  if (status < 0) {
    perror("sendto()");
  }

  //printf("message sent.\n");
  memset(buffer, 0, TAILLE_BUFFER);

  // shutdown socket
  shutdown(sockUDP, 2);
  // on ferme la socket
  close(sockUDP);












  struct Joueurs joueur;

  // identification socket d'écoute du serveur
  static struct sockaddr_in addr_serveur;

  // identifiants de la machine où tourne le serveur
  struct hostent *host_serveur;

  // socket locale coté client
  int sock;

  char name[256];
  gethostname(name, 1023);

  // message à envoyer au serveur
  char *message = "Nouveau joueur a rejoint le serveur...";

  // chaîne où sera écrit le message reçu
  char reponse[TAILLE_BUFFER];

  // nombre d'octets envoyés/reçus
  int nb_octets;

  sock = creerSocketTCP();
  if(sock == -1){
    perror("erreur creation socket");
    exit(1);
  }

  // récupération identifiants de la machine serveur
  host_serveur = gethostbyname(name);

  if (host_serveur==NULL)  {
    perror("erreur récupération adresse serveur\n");
    exit(1);
  }

  // création de l'identifiant de la socket d'écoute du serveur
  bzero((char *) &addr_serveur, sizeof(addr_serveur));
  addr_serveur.sin_family = AF_INET;
  addr_serveur.sin_port = htons(PORT);
  memcpy(&addr_serveur.sin_addr.s_addr, host_serveur->h_addr, host_serveur->h_length);

  // connexion de la socket client locale à la socket coté serveur
  if (connect(sock,(struct sockaddr *)&addr_serveur, sizeof(struct sockaddr_in)) == -1) {
    perror("erreur connexion serveur: ");
    exit(1);
  }
  printf("Connection établie...\n");

  // connexion etablie, on envoie le message
  nb_octets = write(sock, message, strlen(message)+1);

  // on attend la réponse du serveur
  nb_octets = read(sock, reponse, TAILLE_BUFFER);
  printf("identifiant attribué : %s\n", reponse);

  // on ferme la socket
  close(sock);
  printf("Connection fermé...\n");
  return (0);
}
