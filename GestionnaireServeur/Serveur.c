#include "Serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#define TAILLE_BUFFER 1024

// Initialiser le serveur
int initialiser_serveur(const char *adresse_ip, int port)
{
    int serveur_socket;
    struct sockaddr_in adresse_serveur;

    // Créer le socket
    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serveur_socket == -1)
    {
        perror("Erreur lors de la création du socket");
        return EXIT_FAILURE;
    }

    // Configurer l'adresse du serveur
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(port);
    adresse_serveur.sin_addr.s_addr = inet_addr(adresse_ip);

    // Lier le socket à l'adresse et au port
    if (bind(serveur_socket, (struct sockaddr *)&adresse_serveur, sizeof(adresse_serveur)) == -1)
    {
        perror("Erreur lors du bind");
        close(serveur_socket);
        return EXIT_FAILURE;
    }

    // Mettre le serveur en mode écoute
    if (listen(serveur_socket, MAX_CLIENTS) == -1)
    {
        perror("Erreur lors de l'écoute");
        close(serveur_socket);
        return EXIT_FAILURE;
    }

    return serveur_socket;
}

// Accepter les connexions des clients
int accepter_connexion(int serveur_socket)
{
    struct sockaddr_in adresse_client;
    socklen_t taille_adresse = sizeof(adresse_client);

    // Accepter une connexion
    int client_socket = accept(serveur_socket, (struct sockaddr *)&adresse_client, &taille_adresse);
    if (client_socket == -1)
    {
        perror("Erreur lors de l'acceptation d'une connexion");
        return EXIT_FAILURE;
    }

    printf("Connexion acceptée d'un client\n");
    return client_socket;
}


// Recevoir le nom d'un client
char* recevoir_nom_client(int client_socket){
    char buffer[TAILLE_NOM_JOUEUR];
    ssize_t nom_recu = recv(client_socket, buffer, TAILLE_NOM_JOUEUR -1, 0);

    if(nom_recu > 0){
        buffer[nom_recu] = '\0'; //Assurer que le nom est nul terminé
        printf("Nom du client connecté : %s\n", buffer);

        // Allocation mémpire pour le nom
        char *nom = malloc((nom_recu + 1) * sizeof(char));
        if(nom == NULL){
            perror("Erreur d'allocation mémoire pour le nom du client");
            exit(EXIT_FAILURE);
        }
        strcpy(nom, buffer);
        return nom;
    }else if(nom_recu == 0){
        printf("Le client a fermé la connexion avant d'envoyé son nom\n");
    }else{
        perror("Erreur lors de la réception du nom du client");
    }
    return NULL; // retourner NULL dans le cas d'erreur
}

// Distribuer les cartes aux joueurs
void distribuer_cartes_clients(Etats_Jeu *jeu, Client *clients, int nb_joueurs)
{
    char buffer[TAILLE_BUFFER];

    // Distribuer les cartes aux joueurs avec la méthode du jeu
    int pile_indice = 0;
    demarrer_manche(jeu, jeu->cartes, &pile_indice);
    //distribuer_cartes(jeu, jeu->cartes, &pile_indice);

    // Envoyer les cartes aux joueurs via le réseau
    for (int i = 0; i < nb_joueurs; i++)
    {
        int nb_cartes = clients[i].joueur.nb_cartes;
        int *cartes_a_envoyees = malloc(nb_cartes * sizeof(int));

        // Tableau des numéros de cartes à envoyer
        for (int j = 0; j < nb_cartes; j++)
        {
            cartes_a_envoyees[j] = clients[i].joueur.main[j].numero;
        }

        // Envoyer les cartes au joueur
        if (send(clients[i].socket, cartes_a_envoyees, nb_cartes * sizeof(int), 0) < 0)
        {
            perror("Erreur lors de l'envoi des cartes au client");
            close(clients[i].socket);
            free(cartes_a_envoyees);
            continue;
        }
        else
        {
            snprintf(buffer, TAILLE_BUFFER, "Cartes distribuées : [%d cartes]\n", nb_cartes);
            send(clients[i].socket, buffer, strlen(buffer), 0);
        }
    }

    printf("Cartes distribuées à tous les joueurs.\n");
}

// Gérer les tours de jeu
void gerer_tours(Etats_Jeu *jeu,StatPartie *stats, Client *clients, int nb_joueurs, int *indice_pile)
{
    fd_set sockets_lecture;
    // timeout : délai de réponse
    // temps_initial, temps_final : pour calculer le délai de réponse de chaque joueur
    struct timeval timeout, temps_initial, temps_final;
    double temps_reaction; // Pour strocker le temps de réaction
    char buffer[TAILLE_BUFFER];
    int max_fd = 0;

    // Définir le délai de (5 secondes pour poser une carte)
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Capturer le temps initial avant de commencer à écouter
    gettimeofday(&temps_initial, NULL);

    while (*indice_pile < jeu->nb_cartes && jeu->vies > 0)
    {
        // Initialiser le set de sockets
        FD_ZERO(&sockets_lecture);
        for (int i = 0; i < nb_joueurs; i++)
        {
            FD_SET(clients[i].socket, &sockets_lecture);
            if (clients[i].socket > max_fd)
            {
                max_fd = clients[i].socket;
            }
        }

        printf("Les joueurs ont 5 secondes pour jouer une carte\n");

        // Attendre qu'un ou plusieurs sockets soient prêts
        int activite = select(max_fd + 1, &sockets_lecture, NULL, NULL, &timeout);

        if (activite < 0)
        {
            perror("Erreur avec select");
            break;
        }
        else if (activite == 0)
        {
            printf("Temps écoulé, aucun joueur n'a joué.\n");
            jeu->vies--;
            snprintf(buffer, TAILLE_BUFFER, "Temps écoulé ! Vous perdez une vie. Vies restantes : %d\n", jeu->vies);
            for (int i = 0; i < nb_joueurs; i++)
            {
                send(clients[i].socket, buffer, strlen(buffer), 0);
            }
            return; // Recommencer la manche
        }

        for (int i = 0; i < nb_joueurs; i++)
        {
            // Recevoir une carte d'un joueur
            if (FD_ISSET(clients[i].socket, &sockets_lecture))
            {
                int carte_jouee;
                if (recv(clients[i].socket, &carte_jouee, sizeof(int), 0) > 0)
                {
                    printf("%s a joué la carte : %d\n", clients[i].joueur.nom, carte_jouee);

                    // Capturer le temp de l'envoie de la réponse
                    gettimeofday(&temps_final, NULL);
                    temps_reaction = (temps_final.tv_sec - temps_initial.tv_sec) + 
                                    (temps_final.tv_usec - temps_final.tv_usec) / 1000000.0;

                    // Vérifier la carte jouée
                    if (!verifier_manche(jeu, carte_jouee, indice_pile))
                    {
                        // Informer les jouers de l'échec
                        snprintf(buffer, TAILLE_BUFFER, "Manche échouée. vous perdez une vie. \n");
                        for (int j = 0; j < nb_joueurs; j++)
                        {
                            send(clients[j].socket, buffer, strlen(buffer), 0);
                        }

                        // Réinitialiser la manche
                        jeu->vies--;

                        /*****Mettre à jour le valeur qui a causé l'échec******/
                        /*****Mettre à jour le temp de réaction******/
                        mettre_a_jour_statistiques(stats, i, 0, carte_jouee, temps_reaction);
                        return;
                    }

                    // Informer les joueurs que la carte est correcte
                    (*indice_pile)++;
                    snprintf(buffer, TAILLE_BUFFER, "Carte %d correcte ! NEXXTTT.\n", carte_jouee);
                    for (int j = 0; j < nb_joueurs; j++)
                    {
                        send(clients[j].socket, buffer, strlen(buffer), 0);
                    }

                    /*****Mettre à jour le nombre de cartes jouées correctement pour le joueur******/
                    /*****Mettre à jour le temp de réaction******/
                    mettre_a_jour_statistiques(stats, i, 2, 0, temps_reaction);

                    break;
                }
                else
                {
                    perror("Erreur lors de la réception de la carte");
                }
            }
        }
    }

    // Si toutes les cartes ont été jouées correctement
    if(*indice_pile >= jeu->nb_cartes){
        printf("Manche réussie ! \n");
        snprintf(buffer, TAILLE_BUFFER, "Manche réussie ! Préparez-vous pour la suivante.\n");
        for (int i = 0; i < nb_joueurs; i++) {
            send(clients[i].socket, buffer, strlen(buffer), 0);

            /*****Mettre à jour le nombre de manches réussies pour tous mes joueurs******/
            /*****Mettre à jour le nombre total de manches******/
            mettre_a_jour_statistiques(stats, i, 1, 0, 0.0);
        }
    }
}

// Boucle principale du serveur
void boucle_principale(int serveur_socket)
{
    char buffer[TAILLE_BUFFER]; // Buffer pour envoyer des messages aux clients
    Client clients[MAX_CLIENTS] = {0}; // Tableau pour stocker les clients connecté
    int indice_pile = 0; // Nombre de cartes au début
    int nb_clients = 0; // Nombre de clients au début
    Etats_Jeu jeu; // Structure du jeu
    StatPartie stats; // Structure des statistiques du jeu

    // Initialiser l'état du jeu au début
    initialiser_jeu(&jeu, MAX_CLIENTS, NB_VIES, NB_SHURIKENS);

    // Initialiser les statistiques du jeu
    initialiser_stats_partie(&jeu, &stats);

    while (1)
    {
        if (nb_clients < MAX_CLIENTS)
        {
            printf("En attente de joueurs ... \n");
            // Accepter une nouvelle connexion
            int client_socket = accepter_connexion(serveur_socket);

            // Recevoir le nom du client connecté
            char *nom_client = recevoir_nom_client(client_socket);
            if (client_socket > 0 && nom_client != NULL)
            {
                // Ajouter un client et l'initialiser comme joueur
                clients[nb_clients].socket = client_socket;
                strcpy(clients[nb_clients].joueur.nom, nom_client);

            
                initialiser_joueur(&(clients[nb_clients].joueur), nb_clients + 1, nom_client);
                nb_clients++;

                free(nom_client);

                printf("Joueur ajouté. Nombre total : %d\n", nb_clients);
                snprintf(buffer, TAILLE_BUFFER, "Bienvenue %s! En attente des autres joueurs...\n", clients[nb_clients - 1].joueur.nom);
                send(client_socket, buffer, strlen(buffer), 0);
            }else{
                close(client_socket);
            }
        }

        // Si tous les joueurs sont connectés, démarrer la partie
        if (nb_clients == MAX_CLIENTS)
        {
            printf("Démarrer la manche ... \n");

            // Distribuer les cartes au client
            distribuer_cartes_clients(&jeu, clients, nb_clients);

            // Gérer les tours de jeu
            while (jeu.vies > 0)
            {
                printf("Début d'une nouvelle manche\n");

                gerer_tours(&jeu, &stats, clients, nb_clients, &indice_pile);

                // Vérifier si la manche est terminée
                if (indice_pile >= jeu.nb_cartes)
                {
                    printf("Manche réussie ! Distribution de cartes supplémentaires.\n");
                    //jeu.niveau++; // Passage au niveau suivant
                    //indice_pile = 0; // Réinitialiser l'indice de pile
                    distribuer_cartes_clients(&jeu, clients, nb_clients);
                }
            }
            /* Ajouter la condition dans le cas pù les joueurs ont décidé de terminer la partie */

            // Si les joueurs n'ont plus de vies, la partie est terminée
            printf("Partie terminée. Les joueurs n'ont plus de vies.\n");
            strcpy(buffer, "Partie terminée. Vous avez perdu !\n");
            for (int i = 0; i < nb_clients; i++)
            {
                send(clients[i].socket, buffer, strlen(buffer), 0);
                close(clients[i].socket); // Fermer les sockets des clients
            }

            /* Sauvgarder les statistiques*/
            sauvegarder_statistiques(&stats);

            break; // Quitter la boucle principale
        }
    }

    close(serveur_socket); // Fermer la socket du serveur
}
