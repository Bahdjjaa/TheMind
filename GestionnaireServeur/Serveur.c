#include "Serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>

#define TAILLE_BUFFER 1024

pthread_mutex_t mutex_envoi = PTHREAD_MUTEX_INITIALIZER;
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
char *recevoir_nom_client(int client_socket)
{
    char buffer[TAILLE_NOM_JOUEUR];
    ssize_t nom_recu = recv(client_socket, buffer, TAILLE_NOM_JOUEUR - 1, 0);

    if (nom_recu > 0)
    {
        buffer[nom_recu] = '\0'; // Assurer que le nom est nul terminé
        printf("Nom du client connecté : %s\n", buffer);

        // Allocation mémpire pour le nom
        char *nom = malloc((nom_recu + 1) * sizeof(char));
        if (nom == NULL)
        {
            perror("Erreur d'allocation mémoire pour le nom du client");
            exit(EXIT_FAILURE);
        }
        strcpy(nom, buffer);
        return nom;
    }
    else if (nom_recu == 0)
    {
        printf("Le client a fermé la connexion avant d'envoyé son nom\n");
    }
    else
    {
        perror("Erreur lors de la réception du nom du client");
    }
    return NULL; // retourner NULL dans le cas d'erreur
}

// Distribuer les cartes aux joueurs (FIXED)
void distribuer_cartes_clients(Etats_Jeu *jeu, int nb_joueurs)
{
    char buffer[TAILLE_BUFFER];
    Carte *pile = malloc(NB_CARTES_TOTAL * sizeof(Carte));
    if (pile == NULL)
    {
        perror("Erreur d'allocation mémoire pour le pile de cartes");
        exit(EXIT_FAILURE);
    }

    // Générer et mélanger les cartes
    generer_cartes(pile);
    melanger_cartes(pile, NB_CARTES_TOTAL);

    // Démarrer la manche
    int pile_indice = 0;
    demarrer_manche(jeu, pile, &pile_indice);

    // Distribuer les cartes aux joueurs
    for (int i = 0; i < nb_joueurs; i++)
    {
        Joueur *joueur = &jeu->joueurs[i];
        int nb_cartes = joueur->nb_cartes;
        int offset = 0;
        int *cartes_envoyees = malloc(nb_cartes * sizeof(int));
        if (cartes_envoyees == NULL)
        {
            perror("Erreur d'allocation mémoire pour les cartes envoyées");
        }

        // Récupérer les numéros des cartes
        for (int j = 0; j < nb_cartes; j++)
        {
            cartes_envoyees[j] = joueur->main[j].numero;
        }

        offset += sprintf(buffer + offset, "CARTES;%d", nb_cartes);
        // Débogage
        printf("Nombre de cartes envoyées : %d\n", nb_cartes);

        for (int k = 0; k < nb_cartes; k++)
        {
            offset += sprintf(buffer + offset, ";%d", cartes_envoyees[k]);
        }

        // Débogage
        printf("Cartes distribuées au joueur %d : ", i + 1);
        for (int l = 0; l < nb_cartes; l++)
        {
            printf("%d ", cartes_envoyees[l]);
        }
        printf("\n");

        // Envoi des cartes au joueur
        if (send(joueur->socket, buffer, strlen(buffer), 0) < 0)
        {
            perror("Erreur lors de l'envoie des cartes\n");
            free(cartes_envoyees);
            exit(EXIT_FAILURE);
        }

        free(cartes_envoyees);
    }

    // Libération de la pile
    free(pile);
    printf("Distribution des cartes terminée.\n");

    /*const char *top_depart = "TOP_DEPART";
    for(int i = 0; i < nb_joueurs; i++){
        Joueur *joueur = &jeu->joueurs[i];
        if(send(joueur->socket, top_depart, strlen(top_depart), 0) < 0){
            perror("Erreur lors d el'envoi du signal de départ");
            exit(EXIT_FAILURE);
        }
    }*/
}

// Gérer les tours de jeu
void gerer_tours(Etats_Jeu *jeu, StatPartie *stats, int nb_joueurs)
{
    fd_set sockets_lecture;
    // timeout : délai de réponse
    // temps_initial, temps_final : pour calculer le délai de réponse de chaque joueur
    struct timeval timeout, temps_initial, temps_final;
    double temps_reaction; // Pour strocker le temps de réaction
    char buffer[TAILLE_BUFFER];
    int max_fd = 0;
    int nb_cartes_jouees = 0;

    // Capturer le temps initial avant de commencer à écouter
    gettimeofday(&temps_initial, NULL);

    while (nb_cartes_jouees < jeu->nb_cartes && jeu->vies > 0)
    {
        // Initialiser le set de sockets
        FD_ZERO(&sockets_lecture);
        for (int i = 0; i < nb_joueurs; i++)
        {
            FD_SET(jeu->joueurs[i].socket, &sockets_lecture);
            if (jeu->joueurs[i].socket > max_fd)
            {
                max_fd = jeu->joueurs[i].socket;
            }
        }

        printf("Les joueurs ont 20 secondes pour jouer une carte\n");

        // Définir le délai de (20 secondes pour poser une carte)
        timeout.tv_sec = 20;
        timeout.tv_usec = 0;

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
                send(jeu->joueurs[i].socket, buffer, strlen(buffer), 0);
            }
            jeu->niveau = 1;

            if (jeu->vies == 0)
            {
                // Si les joueurs n'ont plus de vies, la partie est terminée
                printf("Partie terminée. Les joueurs n'ont plus de vies.\n");
                strcpy(buffer, "Partie terminée. Vous avez perdu !\n");
                for (int i = 0; i < nb_joueurs; i++)
                {
                    send(jeu->joueurs[i].socket, buffer, strlen(buffer), 0);
                }
            }
            return; // Recommencer la manche
        }

        for (int i = 0; i < nb_joueurs; i++)
        {
            // Recevoir une carte d'un joueur
            if (FD_ISSET(jeu->joueurs[i].socket, &sockets_lecture))
            {
                int carte_jouee;
                ssize_t recu = recv(jeu->joueurs[i].socket, &carte_jouee, sizeof(int), 0);
                if (recu > 0)
                {
                    printf("%s a joué la carte : %d\n", jeu->joueurs[i].nom, carte_jouee);

                    // Capturer le temp de l'envoie de la réponse
                    gettimeofday(&temps_final, NULL);
                    temps_reaction = (temps_final.tv_sec - temps_initial.tv_sec) +
                                     (temps_final.tv_usec - temps_final.tv_usec) / 1000000.0;

                    if (carte_jouee == -1)
                    {
                        snprintf(buffer, TAILLE_BUFFER, "%s a décidé de terminer la partie\n", jeu->joueurs[i].nom);
                        for (int j = 0; j < nb_joueurs; j++)
                        {
                            if (j != i)
                            {
                                send(jeu->joueurs[j].socket, buffer, strlen(buffer), 0);
                            }
                        }
                        jeu->vies = 0;
                        return;
                    }
                    // Vérifier la carte jouée
                    if (!verifier_manche(jeu, carte_jouee, &nb_cartes_jouees))
                    {
                        // Réinitialiser la manche
                        jeu->vies--;

                        if (jeu->vies > 0)
                        {
                            /*****Mettre à jour le valeur qui a causé l'échec******/
                            /*****Mettre à jour le temp de réaction******/
                            mettre_a_jour_statistiques(stats, i, 0, carte_jouee, temps_reaction);

                            // Informer les jouers de l'échec
                            snprintf(buffer, TAILLE_BUFFER, "%s a joué la carte %d.. Manche échouée.\nvous perdez une vie. Vies restantes : %d\nRedistribution de cartes dans 3 secondes", jeu->joueurs[i].nom, carte_jouee, jeu->vies);
                            nb_cartes_jouees = 0;
                            for (int j = 0; j < nb_joueurs; j++)
                            {
                                send(jeu->joueurs[j].socket, buffer, strlen(buffer), 0);
                            }
                            sleep(3);

                            jeu->niveau = 1;
                            return;
                        }else{
                            // Informer les jouers de l'échec
                            snprintf(buffer, TAILLE_BUFFER, "%s a joué la carte %d..\n===PARTIE TERMINÉE===\nVous n'avez plus de vies\n", jeu->joueurs[i].nom, carte_jouee);
                            nb_cartes_jouees = 0;
                            for (int j = 0; j < nb_joueurs; j++)
                            {
                                send(jeu->joueurs[j].socket, buffer, strlen(buffer), 0);
                            }
                            /*****Mettre à jour le valeur qui a causé l'échec******/
                            /*****Mettre à jour le temp de réaction******/
                            mettre_a_jour_statistiques(stats, i, 0, carte_jouee, temps_reaction);
                            return;
                        }
                    }

                    // Informer les joueurs que la carte est correcte
                    nb_cartes_jouees++;
                    snprintf(buffer, TAILLE_BUFFER, "%s a joué une carte %d correcte ! Continuez.\n", jeu->joueurs[i].nom, carte_jouee);
                    for (int j = 0; j < nb_joueurs; j++)
                    {
                        send(jeu->joueurs[j].socket, buffer, strlen(buffer), 0);
                    }

                    /*****Mettre à jour le nombre de cartes jouées correctement pour le joueur******/
                    /*****Mettre à jour le temp de réaction******/
                    mettre_a_jour_statistiques(stats, i, 2, 0, temps_reaction);

                    // Si toutes les cartes ont été jouées correctement
                    if (nb_cartes_jouees >= jeu->nb_cartes)
                    {
                        printf("Manche réussie ! \n");
                        snprintf(buffer, TAILLE_BUFFER, "Manche réussie ! Préparez-vous pour la suivante.\nRedistibution de cartes dans 3 secondes\n");
                        for (int j = 0; j < nb_joueurs; j++)
                        {
                            send(jeu->joueurs[j].socket, buffer, strlen(buffer), 0);

                            /*****Mettre à jour le nombre de manches réussies pour tous mes joueurs******/
                            /*****Mettre à jour le nombre total de manches******/
                            mettre_a_jour_statistiques(stats, i, 1, 0, 0.0);
                        }
                        sleep(3);
                        jeu->niveau++;
                        free(jeu->cartes);
                    }
                }
                else if (recu == 0)
                {
                    printf("Joueur %s s'est déconnecté.\n", jeu->joueurs[i].nom);
                    close(jeu->joueurs[i].socket);
                    FD_CLR(jeu->joueurs[i].socket, &sockets_lecture);
                    snprintf(buffer, TAILLE_BUFFER, "%s s'est déconnecté. Fin de la partie\n", jeu->joueurs[i].nom);
                    for (int j = 0; j < nb_joueurs; j++)
                    {
                        if (i != j)
                        {
                            send(jeu->joueurs[j].socket, buffer, strlen(buffer), 0);
                        }
                    }
                    jeu->vies = 0;
                    break;
                }
                else
                {
                    perror("Erreur lors de la réception de la carte");
                }
            }
        }
    }
}

// Boucle principale du serveur
void boucle_principale(int serveur_socket, int nb_joueurs)
{
    char buffer[TAILLE_BUFFER]; // Buffer pour envoyer des messages aux clients
    int nb_clients = 0;         // Nombre de clients au début
    Etats_Jeu jeu;              // Structure du jeu
    StatPartie stats;           // Structure des statistiques du jeu

    // Initialiser l'état du jeu au début
    initialiser_jeu(&jeu, nb_joueurs);

    // Initialiser les statistiques du jeu
    initialiser_stats_partie(&jeu, &stats);

    while (1)
    {
        // Accepter les connexions
        if (nb_clients < nb_joueurs)
        {
            printf("En attente de joueurs ... \n");
            // Accepter une nouvelle connexion
            int client_socket = accepter_connexion(serveur_socket);

            // Recevoir le nom du client connecté
            char *nom_client = recevoir_nom_client(client_socket);
            if (client_socket > 0 && nom_client != NULL)
            {
                // Ajouter un client et l'initialiser comme joueur
                jeu.joueurs[nb_clients].socket = client_socket; // Affecter la valeur du socket à joueur.socket
                strcpy(jeu.joueurs[nb_clients].nom, nom_client);
                strcpy(stats.stat_joueurs[nb_clients].joueur.nom, nom_client); // Mise à jour du nom

                initialiser_joueur(&(jeu.joueurs[nb_clients]), nom_client);
                nb_clients++;

                free(nom_client);

                printf("Joueur ajouté. Nombre total : %d\n", nb_clients);
                snprintf(buffer, TAILLE_BUFFER, "Bienvenue %s! En attente des autres joueurs...\n", jeu.joueurs[nb_clients - 1].nom);
                send(client_socket, buffer, strlen(buffer), 0);
            }
            else
            {
                close(client_socket);
            }
        }

        // Si tous les joueurs sont connectés, démarrer la partie
        if (nb_clients == nb_joueurs)
        {
            printf("Démarrer la manche ... \n");
            // Gérer les tours de jeu
            while (jeu.vies > 0)
            {
                // Distribuer les cartes au client
                distribuer_cartes_clients(&jeu, nb_clients);
                gerer_tours(&jeu, &stats, nb_clients);
            }
            break;
        }
    }
    /* Sauvgarder les statistiques*/
    sauvegarder_statistiques(&stats);

    for(int i = 0; i < nb_clients; i++){
        close(jeu.joueurs[i].socket);
    }
    close(serveur_socket); // Fermer la socket du serveur
}
