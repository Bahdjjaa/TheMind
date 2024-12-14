#include "GestionnaireClient.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Calculer les délai d'attente et envoyer la carte la plus petite
void jouer_carte_robot(int sockfd, Carte *cartes, int nb_cartes)
{
    int min_val = 101; // Une valeur impossible pour initialiser la recherche de la carte minimum
    int index_min = -1;

    // Trouver la carte non jouée la plus petite
    for (int i = 0; i < nb_cartes; i++)
    {
        if (cartes[i].est_jouee == 0 && cartes[i].num < min_val)
        {
            min_val = cartes[i].num;
            index_min = i;
        }
    }

    if (index_min == -1)
    {
        printf("Aucune carte disponible à jouer.\n");
        return;
    }

    // Calculer le délai d'attente (en secondes)
    double delai = 1 + (7.0 / 98) * (min_val - 1); // Délai proportionnel entre 1 et 8 secondes
    printf("Le robot attend %.2f secondes avant de jouer la carte : %d\n", delai, cartes[index_min].num);
    usleep(delai * 1000000); // Convertir le délai en microsecondes pour usleep()

    // Jouer la carte
    printf("Le robot joue la carte : %d\n", cartes[index_min].num);
    if (send(sockfd, &cartes[index_min].num, sizeof(int), 0) < 0)
    {
        perror("Erreur lors de l'envoi de la carte");
        return;
    }

    // Marquer la carte comme jouée
    cartes[index_min].est_jouee = 1;
}


void boucle_principale_client_robot(int sockfd, const char *nom_robot)
{
    char buffer[BUFSIZ];
    Carte cartes[50];
    int nb_cartes = 0;

    // Initialiser le joueur
    Joueur robot;
    definir_nom_joueur(&robot, 0, nom_robot);

    // Envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, robot.nom);

    while (1)
    {
        // Réception des messages du serveur
        memset(buffer, 0, sizeof(buffer));
        int bytes_recus = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_recus > 0)
        {
            buffer[bytes_recus] = '\0';
            printf("\nMessage > : %s\n", buffer);

            // Si le serveur envoie des cartes
            char *token = strtok(buffer, ";");
            if (strcmp(token, "CARTES") == 0)
            {
                // Réinitialiser les cartes
                for (int i = 0; i < 50; i++)
                {
                    cartes[i].num = 0;
                    cartes[i].est_jouee = 0;
                }

                token = strtok(NULL, ";");
                nb_cartes = atoi(token);

                printf("Niveau actuel du robot : %d\n", nb_cartes);

                for (int i = 0; i < nb_cartes; i++)
                {
                    token = strtok(NULL, ";");
                    if (token == NULL)
                    {
                        printf("Message invalide : valeur de carte manquante\n");
                    }
                    cartes[i].num = atoi(token);
                    cartes[i].est_jouee = 0; // Par défaut, aucune carte n'est jouée
                }

                printf("Cartes du robot: ");
                for (int i = 0; i < nb_cartes; i++)
                {
                    printf("%d ", cartes[i].num);
                }
                printf("\n");

                // Le robot joue automatiquement sa carte
                jouer_carte_robot(sockfd, cartes, nb_cartes);
            }
            else if (strstr(buffer, "Partie terminée") != NULL)
            {
                printf("Fin de la partie.\n");
                break;
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

        // Le robot joue automatiquement sa carte
        jouer_carte_robot(sockfd, cartes, nb_cartes);
    }

    close(sockfd);
}
