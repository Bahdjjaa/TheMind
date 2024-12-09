#ifndef GESTIONNAIRE_CLIENT_H
#define GESTIONNAIRE_CLIENT_H

//fonction pour initialiser le socket et se connecter au serveur
int initialiser_socket(const char *adress_ip, int port);

//fonction pour envoyer une action au serveur
void envoyer_action(int sockfd, const char *nom, int carte);

//Fonction pour recevoir l'etat du jeu ou les transformation du serveur
void recevoir_etat(int sockfd);

//void fermer_connexion(int sockfd);

#endif