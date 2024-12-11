#ifndef GESTIONNAIRE_CLIENT_H
#define GESTIONNAIRE_CLIENT_H

//fonction pour initialiser le socket et se connecter au serveur
int initialiser_socket(const char *adress_ip, int port);

//Definir le nom du joueur
void definir_nom_joueur(Joueur *joueur, int joueur_id, const char *nom);

//Envoyer le nom du joueur
void envoyer_nom_joueur(int sockfd, const char *nom_joueur);

//boucle principale du joueur
 void boucle_principale_client(int sockfd , const char *nom_joueur)

#endif