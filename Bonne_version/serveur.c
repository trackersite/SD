#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "serveur.h"

int main (int argc, char *argv[]) {
  int sock, status, socklen;
  char buffer[TAILLE_BUFFER];
  char *donnees_client[3];

  


  return 0;
}
