#include "GestionnaireClient.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//boucle principale pour le joueur robot
void boucle_principale_client_robot(int sockfd, const char *nom){
    char buffer[BUFSIZ];
    int cartes_recues[BUFSIZ];

    printf("Joueur robot connecté au serveur .\n");

    //Definir le joueur (initialiser la structure)
    Joueur joueur;
    definir_nom_joueur(&joueur, 1, nom);

    //envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, joueur.nom);

    while(1){
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer), 0)>0){
            printf("Serveur: %s\n", buffer);

            if(strstr(buffer, "Cartes distribuées") != NULL){
                int nb_cartes;
                recv(sockfd, &nb_cartes, sizeof(int), 0);
                recv(sockfd, cartes_recues, nb_cartes *sizeof(int), 0);

                printf("Cartes reçues par le robot : ");
                for (int i = 0; i < nb_cartes; i++) {
                    printf("%d", cartes_recues[i]);
                }
                printf("\n");

                //Choisir automatiquement la plus petite carte
                int carte_jouee = cartes_recues[0];
                for (int i=1; i < nb_cartes; i++) {
                    if (cartes_recues[i] < carte_jouee) {
                        carte_jouee = cartes_recues[i];
                    }
                }

                printf("Robot joue la carte : %d\n", carte_jouee);

                if (send(sockfd, &carte_jouee, sizeof(int), 0) < 0){
                    perror("Erreur lors de l'envoi de la carte");
                    break;
                }
            }
        } else if(strstr(buffer, "Partie terminée") != NULL){
            printf("Fin de la partie .\n");
            break;
        }
    }
    
    close (sockfd);
}