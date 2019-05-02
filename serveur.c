#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "serveur.h"
#include "common.h"

int main(int argc, char const *argv[]) {
  // adresse socket coté client
  static struct sockaddr_in addr_client;
  // adresse socket locale
  static struct sockaddr_in addr_serveur;
  // longueur adresse
  int lg_addr;
  // socket d'écoute et de service
  int socket_ecoute, socket_service;
  // buffer qui contiendra le message reçu
  char message[TAILLE_BUFFER_SERVEUR];
  // chaîne reçue du client
  char *chaine_recue;
  // chaîne renvoyée au client
  char *reponse = "bien recu";
  char *id = "1";
  // nombre d'octets reçus ou envoyés
  int nb_octets;

  // création socket TCP d'écoute
  socket_ecoute = creerSocketTCP(SERVER_PORT_SERVER);
  if (socket_ecoute == -1) {
    perror("creation socket");
    exit(1);
  }

  // configuration socket écoute : 5 connexions max en attente
  if (listen(socket_ecoute, 4) == -1) {
    perror("erreur listen");
    exit(1);
  }

  // on attend la connexion du client
  lg_addr = sizeof(struct sockaddr_in);
  socket_service = accept(socket_ecoute,(struct sockaddr *)&addr_client, &lg_addr);
  if (socket_service == -1) {
    perror("erreur accept");
    exit(1);
  }

  // la connexion est établie, on attend les données envoyées par le client
  nb_octets = read(socket_service, message, TAILLE_BUFFER_SERVEUR);

  // affichage du message reçu
  chaine_recue =(char *)malloc(nb_octets * sizeof(char));

  memcpy(chaine_recue, message, nb_octets);
  printf("%s\n", chaine_recue);

  // on envoie la réponse au client
  write(socket_service, id, strlen(reponse)+1);

  // on ferme les sockets
  close(socket_service);
  close(socket_ecoute);

  return(0);
}
