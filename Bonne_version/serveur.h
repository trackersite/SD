#ifndef SERVEUR_H
#define SERVEUR_H

#include "common.h"

struct clients_connectes {
  char *pseudo;
  char *adresse;
  char *port;
};

#endif
