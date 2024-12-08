#ifndef SERVEUR_H
#define SERVER_H

#include "jeu.h"

// Méthode pour initialiser le serveur
int initialiser_serveur(const char *adresse_ip, int port);

// Méthode pour accepter les connexions clieint
int accespter_connexion(int serveur_socket);

// Méthode pour envoyer les cartes au joueurs
void distribuer_cartes(int* cartes, int nb_cartes, int client_socket);

// Méthode pour la gestion les tours de jeu
void gerer_tour(int *cartes, int nb_cartes, int client_socket);

// Méthode de gestion de temps pour chaque tour
void gerer_temps(int cleint_socket);

//Boucle principale du serveur
void boucle_principale(int serveur_socket);

#endif