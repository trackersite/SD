#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include "common.h"

 int creerSocketTCP() {
   // adresse socket coté client
   static struct sockaddr_in addr_client;
   // adresse socket locale
   static struct sockaddr_in addr_serveur;
   // longueur adresse
   int lg_addr;
   // socket d'écoute
   int socket_ecoute;

   // création socket TCP d'écoute
   if ((socket_ecoute = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
     perror("creation socket");
     exit(1);
   }

   // liaison de la socket d'écoute sur le port
   bzero((char *) &addr_serveur, sizeof(addr_serveur));
   addr_serveur.sin_family = AF_INET;
   addr_serveur.sin_port = htons(0);
   addr_serveur.sin_addr.s_addr=htonl(INADDR_ANY);

   if (bind(socket_ecoute, (struct sockaddr*)&addr_serveur, sizeof(addr_serveur))== -1 ) {
     perror("erreur bind socket écoute");
     exit(1);
   }



   return socket_ecoute;
 }
 char *communication_multicast() {
   char *donnees_client[3];
   struct sockaddr_in saddr;
   struct ip_mreq imreq;
   int identifier = 1;
   char *client_adresse;
   char *client_port;
   char *client_pseudo;

   // set content of struct saddr and imreq to zero
   memset(&saddr, 0, sizeof(struct sockaddr_in));
   memset(&imreq, 0, sizeof(struct ip_mreq));

   // open a UDP socket
   sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
   if ( sock < 0 )
     perror("Error creating socket"), exit(0);

   saddr.sin_family = PF_INET;
   saddr.sin_port = htons(PORT); // listen on port 8080
   saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
   status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

   if ( status < 0 )
     perror("Error binding socket to interface"), exit(0);

   imreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
   imreq.imr_interface.s_addr = INADDR_ANY; // use DEFAULT interface

   // JOIN multicast group on default interface
   status = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
              (const void *)&imreq, sizeof(struct ip_mreq));

   socklen = sizeof(struct sockaddr_in);

   // receive packet from socket
   status = recvfrom(sock, buffer, TAILLE_BUFFER, 0,
             (struct sockaddr *)&saddr, &socklen);
   printf(" Received : %s\n", buffer);

   // affecter pseudo
   client_pseudo = buffer;

   memset(buffer, 0, TAILLE_BUFFER);
   buffer[0] = identifier + '0';
   identifier++;
   printf("New connection, ip is : %s , port : %d \n" , inet_ntoa(saddr.sin_addr) , ntohs(saddr.sin_port));

   sprintf(client_port, "%d", ntohs(saddr.sin_port));
   // affectation
   client_adresse = inet_ntoa(saddr.sin_addr);
   donnees_client[0] = client_adresse;
   donnees_client[1] = client_port;
   donnees_client[2] = "Test";
   // concatenation
   strcat(*donnees_client, "\n");
   strcat(*donnees_client, client_port);
   strcat(*donnees_client, "\n");
   strcat(*donnees_client, client_pseudo);

     // envoie
   //sendto(sock, *donnees_client, TAILLE_BUFFER, 0, (struct sockaddr *)&saddr, socklen);

   // shutdown socket
   shutdown(sock, 2);
   // close socket
   close(sock);

   return *donnees_client;
 }
