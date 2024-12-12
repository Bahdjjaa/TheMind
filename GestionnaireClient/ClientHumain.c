#include "ClientHumain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void boucle_principale_client_humain(int sockfd, const char *nom_joueur){ 
    char buffer[BUFSIZ];
    int cartes_recues[BUFSIZ];


    //Definir le joueur (initialiser la structure)
    Joueur joueur;
    definir_nom_joueur(&joueur, 1, nom_joueur);

    //envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, joueur.nom);

    while (1)
    {
        // Recevoir un message du serveur
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer), 0) >0)
        {
            printf("Serveur: %s\n", buffer);

            //si le serveur demande de jouer une carte
            if (strstr(buffer, "Cartes distribuées") != NULL)
            {
                //Recevoir les cartes du serveur
                int nb_Cartes; 
                recv(sockfd, &nb_Cartes, sizeof(int), 0);
                recv(sockfd, cartes_recues, nb_Cartes * sizeof(int), 0);

                printf("Vos cartes: ");
                for (int i = 0; i < nb_Cartes; i++)
                {
                    printf("%d", cartes_recues[i]);
                }
                printf("\n");

                //Demander au joueur de choisir un nombre parmi ses cartes
                int carte_jouee = -1;
                while(carte_jouee < 0 || carte_jouee >= nb_Cartes){
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
            }
        }
        else if (strstr(buffer, "Partie terminée") !=NULL)
        {
            // Si le serveur indique la fin de la partie
            printf("Fin de la partie.\n");
            break;
        }
    }

     
    

    close(sockfd); //Fermer la connexion avec le serveur
 }
