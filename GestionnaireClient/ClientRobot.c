#include "GestionnaireClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void boucle_principale_client_robot(int sockfd, const char *nom_robot) {
    char buffer[BUFSIZ];
    Carte cartes[50];
    int nb_cartes = 0;

    // Initialiser le joueur
    Joueur robot;
    definir_nom_joueur(&robot, 0, nom_robot);

    // Envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, robot.nom);

    while (1) {
        // Réception des messages du serveur
        memset(buffer, 0, sizeof(buffer));
        int bytes_recus = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_recus > 0) {
            buffer[bytes_recus] = '\0';
            printf("\nMessage reçu > : %s\n", buffer);

            // Si le serveur envoie des cartes
            if (strncmp(buffer, "CARTES", 6) == 0) {
                // Réinitialiser les cartes
                for (int i = 0; i < 50; i++) {
                    cartes[i].num = 0;
                    cartes[i].est_jouee = 0;
                }

                char *token = strtok(buffer, ";");
                token = strtok(NULL, ";");
                nb_cartes = atoi(token);

                printf("Niveau actuel du robot : %d\n", nb_cartes);

                for (int i = 0; i < nb_cartes; i++) {
                    token = strtok(NULL, ";");
                    if (token == NULL) {
                        printf("Message invalide : valeur de carte manquante\n");
                    }
                    cartes[i].num = atoi(token);
                    cartes[i].est_jouee = 0; // Par défaut, aucune carte n'est jouée
                }

                printf("Cartes du robot: =========\n");
                for (int i = 0; i < nb_cartes; i++) {
                    printf("Carte %d : %d\n", (i + 1), cartes[i].num);
                }
                printf("\n=======================\n");

            } else if (strstr(buffer, "Continuez") != NULL) {
                printf("Reçu 'Continuez' : le robot prépare à jouer.\n");
                printf("Cartes du robot: =========\n");
                for (int i = 0; i < nb_cartes; i++) {
                    printf("Carte %d : %d\n", (i + 1), cartes[i].num);
                }
                printf("\n=======================\n");
            } else if (strstr(buffer, "Partie terminée") != NULL) {
                printf("Fin de la partie.\n");
                break;
            }

            // Jouer la carte la plus petite
            int min_val = 101; // Une valeur impossible pour initialiser
            int index_min = -1;

            for (int i = 0; i < nb_cartes; i++) {
                if (!cartes[i].est_jouee && cartes[i].num < min_val) {
                    min_val = cartes[i].num;
                    index_min = i;
                }
            }

            if (index_min != -1) {
                double delai = 2 + (17.0 / 98) * (min_val - 1); // Calcul du délai
                printf("Le robot attend %.2f secondes avant de jouer la carte : %d\n", delai, cartes[index_min].num);

                usleep((int)(delai * 1e6)); // Convertir secondes en microsecondes

                // Envoyer la carte au serveur
                printf("Le robot joue la carte : %d\n", cartes[index_min].num);
                if (send(sockfd, &cartes[index_min].num, sizeof(int), 0) < 0) {
                    perror("Erreur lors de l'envoi de la carte");
                    break;
                }

                // Marquer la carte comme jouée
                cartes[index_min].est_jouee = 1;
            } else {
                printf("Aucune carte disponible à jouer.\n");
            }
        } else if (bytes_recus == 0) {
            printf("Déconnexion du serveur.\n");
            break;
        } else {
            perror("Erreur lors de la réception.");
            break;
        }
    }

    close(sockfd);
}
