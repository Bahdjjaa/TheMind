#ifndef CLIENTROBOT_H
#define CLIENTROBOT_H

#include "GestionnaireClient.h"

// Méthode pour calculer un temps d'attente et envoyer la carte la plus petite serveur
void jouer_carte_robot(int sockfd, Carte *cartes, int nb_cartes);

//Boucle prinipale du joueur robot
void boucle_principale_client_robot(int sockfd, const char *nom);

#endif 