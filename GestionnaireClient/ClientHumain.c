#include "ClientHumain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

void boucle_principale_client_humain(int sockfd, const char *nom_joueur)
{
    char buffer[BUFSIZ];
    Carte cartes[50];
    int nb_cartes = 0;

    // Initialiser le joueur
    Joueur joueur;
    definir_nom_joueur(&joueur, 1, nom_joueur);

    // Envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, joueur.nom);

    fd_set sockets_lecture;

    while (1)
    {
        // Initialiser le set de sockets
        FD_ZERO(&sockets_lecture);
        FD_SET(STDIN_FILENO, &sockets_lecture); // Pour la saisie utilisateur
        FD_SET(sockfd, &sockets_lecture);       // Pour les messages du serveur
        int max_fd = sockfd;

        // Attendre l'activité sur stdin ou sockfd
        int activite = select(max_fd + 1, &sockets_lecture, NULL, NULL, NULL);

        if (activite < 0)
        {
            perror("Erreur avec select");
            break;
        }

        // Si un message est disponible sur le socket serveur
        if (FD_ISSET(sockfd, &sockets_lecture))
        {
            memset(buffer, 0, sizeof(buffer));
            int bytes_recus = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_recus > 0)
            {
                buffer[bytes_recus] = '\0';
                printf("\nMessage > : %s\n", buffer);

                // Si le serveur demande de jouer une carte
                char *token = strtok(buffer, ";");
                if (strcmp(token, "CARTES") == 0 && strstr(buffer, "PARTIE TERMINÉE") == NULL)
                {
                    // Vider la pile
                    for (int i = 0; i < 50; i++)
                    {
                        cartes[i].num = 0;
                        cartes[i].est_jouee = 0;
                    }

                    token = strtok(NULL, ";");
                    nb_cartes = atoi(token);
                    
                    printf("Vous êtes au niveau %d\n", nb_cartes);

                    for (int i = 0; i < nb_cartes; i++)
                    {
                        token = strtok(NULL, ";");
                        if (token == NULL)
                        {
                            printf("Message invalide : valeur de carte manquante\n");
                        }
                        cartes[i].num = atoi(token);
                        cartes[i].est_jouee = 0;
                    }

                    printf("Vos cartes: ======\n");
                    for (int i = 0; i < nb_cartes; i++)
                    {
                        printf("Carte %d : %d\n", (i + 1), cartes[i].num);
                    }
                    printf("==================\n");
                }
                else if (strstr(buffer, "Partie terminée") != NULL)
                {
                    printf("Fin de la partie.\n");
                    break;
                }
                else if (strstr(buffer, "Continuez") != NULL)
                {
                    printf("Vos cartes: ======\n");
                    for (int i = 0; i < 50; i++)
                    {
                        if (cartes[i].num != 0 && cartes[i].est_jouee != 1)
                        {
                            printf("Carte %d : %d\n", (i + 1), cartes[i].num);
                        }
                    }
                    printf("==================\n");
                }
            }
            else if (bytes_recus == 0)
            {
                printf("Déconnexion du serveur.\n");
                break;
            }
            else
            {
                perror("Erreur lors de la réception.");
                break;
            }
        }

        // Si une saisie utilisateur est disponible
        if (FD_ISSET(STDIN_FILENO, &sockets_lecture))
        {
            char saisie[100];

            if (fgets(saisie, sizeof(saisie), stdin) != NULL)
            {
                int choix = atoi(saisie);

                if (choix == -1)
                {
                    printf("Vous avez choisi de terminer la partie\n");
                    break;
                }

                if (choix >= 1 && choix <= nb_cartes)
                {
                    printf("Vous jouez la carte : %d\n", cartes[choix - 1].num);

                    // Envoyer la carte jouée au serveur
                    if (send(sockfd, &cartes[choix - 1], sizeof(int), 0) < 0)
                    {
                        perror("Erreur lors de l'envoi de la carte");
                        break;
                    }

                    // Marquer la carte comme jouée
                    cartes[choix-1].est_jouee = 1;
                }
                else
                {
                    printf("Choix invalide, réessayez.\n");
                }
            }
        }
    }

    close(sockfd);
}
