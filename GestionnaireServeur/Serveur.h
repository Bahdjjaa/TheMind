#ifndef SERVEUR_H
#define SERVER_H

#include "jeu.h"

#define MAX_CLIENTS 2

// Méthode pour initialiser le serveur
int initialiser_serveur(const char *adresse_ip, int port);

// Méthode pour accepter les connexions clieint
int accepter_connexion(int serveur_socket);

// Méthode pour recevoir le nom d'un client
char* recevoir_nom_client(int client_socket);

// Méthode pour envoyer les cartes au joueurs
void distribuer_cartes_clients(Etats_Jeu *jeu, int nb_joueurs);

// Méthode pour la gestion les tours de jeu
void gerer_tours(Etats_Jeu *jeu,StatPartie *stats,int nb_joueurs);

//Boucle principale du serveur
void boucle_principale(int serveur_socket, int nb_joueurs);

#endif