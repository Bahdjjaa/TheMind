#include "GestionnaireClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// Définir la structure du joueur avec le champ vie
typedef struct Joueur {
    int id;
    char nom[50];
} Joueur;


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
    printf("Connecté au serveur %s:%d\n", adress_ip, port);
    return sockfd;
} 

//Définir le nom du joueur 
void definir_nom_joueur(Joueur *joueur, int joueur_id, const char *nom){
    joueur->id= joueur_id;
    strncpy(joueur->nom, nom, 50);
    joueur->nom[49]= '\0';
    printf("nom du joueur %d défini : %s\n",joueur_id,joueur->nom);
}


// Fonction pour envoyer le nom du joueur
void envoyer_nom_joueur(int sockfd, const char *nom_joueur){
    if (send(sockfd, nom_joueur, strlen(nom_joueur) + 1, 0) < 0){
        perror("Erreur lors de l'envoi du nom");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Nom du joueur envoyé au serveur : %s\n", nom_joueur);
}










