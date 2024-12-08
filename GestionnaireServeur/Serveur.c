#include "Serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CLIENTS 4
#define NB_VIES 3
#define NB_SHURIKENS 3
#define TAILLE_BUFFER 1024

// Initialiser le serveur
int initialiser_serveur(const char *adresse_ip, int port){
    int serveur_socket;
    struct sockaddr_in adresse_serveur;

    // Créer le socket
    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(serveur_socket == -1){
        perror("Erreur lors de la création du socket");
        return EXIT_FAILURE;
    }

    // Configurer l'adresse du serveur
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(port);
    adresse_serveur.sin_addr.s_addr = inet_addr(adresse_ip);

    // Lier le socket à l'adresse et au port
    if(bind(serveur_socket, (struct sockaddr *)&adresse_serveur, sizeof(adresse_serveur)) == -1){
        perror("Erreur lors du bind");
        close(serveur_socket);
        return EXIT_FAILURE;
    }

    //Mettre le serveur en mode écoute
    if(listen(serveur_socket, MAX_CLIENTS) == -1){
        perror("Erreur lors de l'écoute");
        close(serveur_socket);
        return EXIT_FAILURE;
    }
}

// Accepter les connexions des clients
int accepter_connexion(int serveur_socket){
    struct sockaddr_in adresse_client;
    socklen_t taille_adresse = sizeof(adresse_client);

    // Accepter une connexion
    int client_socket = accept(serveur_socket, (struct sockaddr *)&adresse_client, &taille_adresse);
    if(client_socket == -1){
        perror("Erreur lors de l'acceptation d'une connexion");
        return EXIT_FAILURE;
    }

    printf("Connexion acceptée d'un client\n");
    return client_socket;
}

// Distribuer les cartes aux joueurs
void distribuer_cartes_clients(Etats_Jeu *jeu, Client *clients, int nb_joueurs){
    char buffer[TAILLE_BUFFER];

    // Distribuer les cartes aux joueurs avec la méthode du jeu
    int pile_indice = 0; 
    distribuer_cartes(jeu, jeu->cartes, &pile_indice);

    // Envoyer les cartes aux joueurs via le réseau
    for(int i = 0; i < nb_joueurs; i++){
        int nb_cartes = clients[i].joueur.nb_cartes;
        int cartes_a_envoyees[nb_cartes];

        // Tableau des numéros de cartes à envoyer
        for(int j = 0; j < nb_cartes; j++){
            cartes_a_envoyees[j] = clients[i].joueur.main[j].numero;
        }

        // Envoyer les cartes au joueur
        if(send(clients[i].socket, cartes_a_envoyees, nb_cartes * sizeof(int), 0) < 0){
            perror("Erreur lors de l'envoi des cartes au client");
            close(clients[i].socket);
            continue;
        }else{
            snprintf(buffer, TAILLE_BUFFER, "Cartes distribuées : [%d cartes]\n", nb_cartes);
            send(clients[i].socket, buffer, strlen(buffer), 0);
        }
    }

    printf("Cartes distribuées à tous les joueurs.\n");

}

// Gérer les tours de jeu
void gerer_tours(Etats_Jeu *jeu, Client *clients, int nb_joueurs, int *indice_pile){
    char buffer[TAILLE_BUFFER];

    for(int i = 0; i < nb_joueurs; i++){
        // Recevoir une carte d'un joueur
        int carte_jouee;
        if(recv(clients[i].socket, &carte_jouee, sizeof(int), 0) < 0){
            perror("Erreur lors de la réception d'une carte");
            continue;  
        }

        printf("Joueur %d a joué la carte : %d\n", clients[i].joueur.id, carte_jouee);

        // Vérifier la carte jouée
        if(!verifier_manche(jeu, carte_jouee, indice_pile)){
            // Informer les jouers de l'échec
            snprintf(buffer, TAILLE_BUFFER, "Manche échouée. vous perdez une vie. \n");
            for(int j = 0; j < nb_joueurs; j++){
                send(clients[j].socket, buffer, strlen(buffer), 0);
            } 

            // Réinitialiser la manche
            jeu->vies--;
            return;
        }

        // Informer les joueurs que la carte est incorrecte
        (*indice_pile)++;
        snprintf(buffer, TAILLE_BUFFER, "Carte %d correcte ! NEXXTTT.\n", carte_jouee);
        for(int j = 0; j < nb_joueurs; j++){
            send(clients[j].socket, buffer,strlen(buffer), 0);
        }
    }
}

// Boucle principale du serveur
void boucle_principale(int serveur_socket){
    char buffer[TAILLE_BUFFER];
    Client clients[MAX_CLIENTS] = {0};
    int indice_pile = 0; // Nombre de cartes au début
    int nb_clients = 0;
    Etats_Jeu jeu;

    initialiser_jeu(&jeu, MAX_CLIENTS, NB_VIES, NB_SHURIKENS);

    while(1){
        if(nb_clients < MAX_CLIENTS){
            // Accepter une nouvelle connexion
            int client_socket = accepter_connexion(serveur_socket);
            if(client_socket > 0){
                // Ajouter un client et l'initialiser comme joueur
                clients[nb_clients].socket = client_socket;
                initialiser_joueur(&(clients[nb_clients].joueur), nb_clients + 1);
                nb_clients++;
                printf("Joueur ajouté. Nombre total : %d\n", nb_clients);
            }
        }

        // Si tous les joueurs sont connectés, démarrer la partie
        if(nb_clients == MAX_CLIENTS){
            printf("Démarrer de la manche ... \n");

            // Distribuer les cartes au client
            distribuer_cartes_clients(&jeu, clients, nb_clients);

            // Gérer les tours de jeu
            while(jeu.vies > 0){
                printf("Début d'une nouvelle manche\n");
                gerer_tours(&jeu, clients, nb_clients, &indice_pile);

                // Vérifier si la manche est terminée
                if(indice_pile >= jeu.nb_cartes){
                    printf("Manche réussie ! Distribution de cartes supplémentaires.\n");
                    jeu.nb_cartes++; // Augmenter le nombre de cartes pour la manche suivante
                    indice_pile = 0; // Réinitialiser l'indice de pile
                    distribuer_cartes_clients(&jeu, clients, nb_clients);
                }
            }

            // Si les joueurs n'ont plus de vies, la partie est terminée
            printf("Partie terminée. Les joueurs n'ont plus de vies.\n");
            strcpy(buffer, "Partie terminée. Vous avez perdu !\n");
             for (int i = 0; i < nb_clients; i++) {
                send(clients[i].socket, buffer, strlen(buffer), 0);
                close(clients[i].socket); // Fermer les sockets des clients
            }
            break; // Quitter la boucle principale

        }
   
    }

    close(serveur_socket); // Fermer la socket du serveur
}
