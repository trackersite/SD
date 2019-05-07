#ifndef COMMON_H
#define COMMON_H

#define PORT 8080
#define TAILLE_BUFFER 200
#define TRUE   1
#define FALSE  0
#define MULTICAST_GROUP "226.1.2.3"
#define MULTICAST_IP "127.0.1.1"

int creerSocketTCP();
char *communication_multicast();

#endif
