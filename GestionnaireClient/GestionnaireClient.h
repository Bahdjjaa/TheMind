#ifndef GESTIONNAIRE_CLIENT_H
#define GESTIONNAIRE_CLIENT_H

#include <netinet/in.h>
//Définir la structure Joueur
typedef struct Joueur{
    int id;
    char nom[50];
}Joueur;

//fonction pour initialiser le socket et se connecter au serveur
int initialiser_socket(const char *adress_ip, int port);

//Definir le nom du joueur
void definir_nom_joueur(Joueur *joueur, int joueur_id, const char *nom);

//Envoyer le nom du joueur
void envoyer_nom_joueur(int sockfd, const char *nom_joueur);

#endif