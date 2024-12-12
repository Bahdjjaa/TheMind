#include "ClientHumain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// Gérer la réception de messages
void *gerer_reception(void *sockfd){
    int client_sock = *(int *)sockfd;
    char buffer[BUFSIZ];

    while (1) {
        memset(buffer, 0, BUFSIZ);
        if (recv(client_sock, buffer, BUFSIZ, 0) <= 0) {
            printf("Déconnexion du serveur ou erreur.\n");
            close(client_sock);
            pthread_exit(NULL);
        }

        printf("Serveur: %s\n", buffer);
    }

    return NULL;
}
void boucle_principale_client_humain(int sockfd, const char *nom_joueur){ 
    char buffer[BUFSIZ];
    int nb_Cartes = 0;
    pthread_t thread_reception;


    //Definir le joueur (initialiser la structure)
    Joueur joueur;
    definir_nom_joueur(&joueur, 1, nom_joueur);

    //envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, joueur.nom);

    // Créer un thread pour gérer la réception de messages
    if(pthread_create(&thread_reception, NULL, gerer_reception, &sockfd) != 0){
        perror("Erreur lors de la création du thread de réception");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Recevoir un message du serveur
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer), 0) > 0)
        {
            printf("Serveur: %s\n", buffer);

            // Si le serveur envoie le niveau
            if(strstr(buffer, "Niveau") != NULL){
                //Recevoir les cartes du serveur
                if(recv(sockfd, &nb_Cartes, sizeof(int), 0) < 0){
                    perror("Erreur lors de la réception du niveau\n");
                    exit(EXIT_FAILURE);
                }
                sscanf(buffer, "Niveau %d", &nb_Cartes);
                printf("\nVous êtes au niveau %d\n", nb_Cartes);
            }

            //si le serveur demande de jouer une carte
            if (strcmp(buffer, "CARTES") == 0)
            {
                int *cartes_recues = malloc(nb_Cartes*sizeof(int));
                if(recv(sockfd, cartes_recues, nb_Cartes * sizeof(int), 0) < 0){
                    perror("Erreur lors de la réception des cartes\n");
                    exit(EXIT_FAILURE);
                }

                printf("\nVos cartes: ");
                for (int i = 0; i < nb_Cartes; i++)
                {
                    printf("%d", cartes_recues[i]);
                }
                printf("\n");

                //Demander au joueur de choisir un nombre parmi ses cartes
                int carte_jouee = -1;
                while(carte_jouee < 0 || carte_jouee > nb_Cartes){
                    printf("Choisissez une carte à jouer (numero entre 0 et %d):", nb_Cartes - 1);
                    scanf("%d", &carte_jouee);

                    //verifier que le choix est dans les limites des indices des cartes
                    if (carte_jouee < 0 || carte_jouee >= nb_Cartes){
                        printf("Choix invalide, veuillez entrer un numéro valide de carte.\n"); 
                    }
                    else{
                        // Le joueur a choisi un index invalide, on peut lui indiquer quelle carte il joue
                        printf("vous jouez la carte : %d\n", cartes_recues[carte_jouee]);
                    }

                }
 
                //Envoyer la carte jouée au serveur
                if (send(sockfd, &cartes_recues[carte_jouee], sizeof(int), 0) < 0)
                {
                    perror("erreur lors de l'envoi de la carte");
                    break;
                }

                free(cartes_recues);
            }
        }
        else if (strstr(buffer, "Partie terminée") !=NULL)
        {
            // Si le serveur indique la fin de la partie
            printf("Fin de la partie.\n");
            close(sockfd);
            break;
        }
    }

    // Attendre la fin du thread de reception
    pthread_join(thread_reception, NULL);
    close(sockfd); //Fermer la connexion avec le serveur
 }
