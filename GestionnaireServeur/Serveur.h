#ifndef SERVEUR_H
#define SERVER_H

#include "jeu.h"

#define MAX_CLIENTS 4
#define NB_VIES 3
#define NB_SHURIKENS 3

typedef struct 
{
    int socket; 
    Joueur joueur;
} Client;

// Méthode pour initialiser le serveur
int initialiser_serveur(const char *adresse_ip, int port);

// Méthode pour accepter les connexions clieint
int accepter_connexion(int serveur_socket);

<<<<<<< HEAD
// Méthode pour recevoir le nom d'un client
char* recevoir_nom_client(int client_socket);

=======
>>>>>>> 170e46b (Modification dans GestionnaireClient)
// Méthode pour envoyer les cartes au joueurs
void distribuer_cartes_clients(Etats_Jeu *jeu, Client *clients, int nb_joueurs);

// Méthode pour la gestion les tours de jeu
void gerer_tours(Etats_Jeu *jeu, Client *clients, int nb_joueurs, int *indice_pile);

//Boucle principale du serveur
void boucle_principale(int serveur_socket);

#endif