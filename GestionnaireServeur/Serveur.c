#include "Serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024

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
int accespter_connexion(int serveur_socket){
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
void distribuer_cartes(int* cartes, int nb_cartes, int client_socket){
    if(send(client_socket, cartes, nb_cartes * sizeof(int), 0) == -1){
        perror("Erreur lors de l'envoi des cartes");
    }else{
        printf("Cartes distribuées au client\n");
    }
}

// Gérer les tours de jeu
void gerer_tour(int *cartes, int nb_cartes, int client_socket){
    int carte_jouee;
    char buffer[BUFFER_SIZE];

    // Recevoir une cartes jouée par le client
    if(recv(client_socket, &carte_jouee, sizeof(int), 0) == -1){
        perror("Erreur lors de la réception de la carte");
        return;
    }

    printf("Carte reçue du joueur : %d\n", carte_jouee);

    //vérifier si la carte est correcte
    if(carte_jouee == cartes[0]){
        printf("Carte correcte !\n");
        //Avancer dans le jeu
        memmove(cartes, cartes+1, (nb_cartes - 1) * sizeof(int));
        nb_cartes--;
    }else{
        printf("Carte incorrecte, manche échouée !\n");
        // Renvoyer une erreur au client
        strcpy(buffer, "Erreur : carte incorrecte");
        send(client_socket, buffer, strlen(buffer), 0);
    }
}

// Boucle principale du serveur
void boucle_principale(int serveur_socket){
    int clients[MAX_CLIENTS] = {0};
    int cartes[100];
    int nb_cartes = 4; // Nombre de cartes au début

    // Générer et mélanger les cartes
    demarrer_manche(cartes, nb_cartes);

    while(1){
        // Accepter une nouvelle connexion
        int client_socket = accespter_connexion(serveur_socket);
        if(client_socket > 0){
            for(int i = 0; i < MAX_CLIENTS; i++){
                if(clients[i] == 0){
                    clients[i] = client_socket;
                    printf("Joueur ajouté à la liste\n");
                    break;
                }
            }

            // Distribuer les cartes au client
            distribuer_cartes(cartes, nb_cartes, client_socket);

            // Gérer les tours 
            gerer_tour(cartes, nb_cartes, client_socket);      
        }
    }
}