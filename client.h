#ifndef CLIENT_H
#define CLIENT_H
#include "common.h"

#define MAX_TAILLE_NOM 80
#define SERVER_PORT_CLIENT 8080
#define TAILLE_BUFFER_CLIENT 200

struct Joueurs {
  int id;
  char pseudo[MAX_TAILLE_NOM];
  int points;
  int adresse_socket;
  int maitre;
};

#endif
