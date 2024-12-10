#include "GestionnaireClient.h"
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Initialisation du socket et connexion au serveur
int initialiser_socket(const char *adress_ip, int port) {
    int sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Erreur lors de la création du socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(adress_ip);


    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
        perror("Erreur de connexion au serveur");
        close(sockfd);
        return -1;
    }
    return sockfd;
} 

//envoi de l'action d'un joueur (nom et carte) au serveur 
void envoyer_action (int sockfd, const char *nom, int carte){
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "NOm: %s, Carte: %d", nom, carte);
    if (send(sockfd, buffer, strlen(buffer), 0)<0){
        perror( "Erreur lors de l'envoi de l'action ");
    }
}

//Réception de l'etat du jeu ou des informations du serveur
void recevoir_etat(int sockfd){
    char buffer[256];
    int n= recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n<0) {
        perror("Erreur lors de la réception des données");
    } else {
        buffer[n] = '\0';
        printf("Etat du jeu: %s\n", buffer);
    }
}

//fermeture de la connexion 
void fermer_connexion (int sockfd){
    close(sockfd);
}