#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include "client.h"
#include "common.h"

int main(int argc, char const *argv[]) {
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
  char reponse[TAILLE_BUFFER_CLIENT];

  // nombre d'octets envoyés/reçus
  int nb_octets;

  sock = creerSocketTCP(0);
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
  addr_serveur.sin_port = htons(SERVER_PORT_CLIENT);
  memcpy(&addr_serveur.sin_addr.s_addr, host_serveur->h_addr, host_serveur->h_length);

  // connexion de la socket client locale à la socket coté serveur
  if (connect(sock,(struct sockaddr *)&addr_serveur, sizeof(struct sockaddr_in)) == -1) {
    perror("erreur connexion serveur");
    exit(1);
  }
  printf("Connection établie...\n");
  printf("Bonjour, veuillez entrer votre pseudo : ");
  fgets(joueur.pseudo, MAX_TAILLE_NOM, stdin);
  printf("Votre pseudo : %s", joueur.pseudo);

  // connexion etablie, on envoie le message
  nb_octets = write(sock, message, strlen(message)+1);

  // on attend la réponse du serveur
  nb_octets = read(sock, reponse, TAILLE_BUFFER_CLIENT);
  printf("identifiant attribué : %s\n", reponse);

  // on ferme la socket
  close(sock);
  printf("Connection fermé...\n");
  return (0);
}
